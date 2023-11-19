#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void fillArray ( int * array, int length ) {
    time_t t;
    srand((unsigned) time(&t));
    for ( int i = 0; i < length; i++ )
        array[i] = rand() % 5 + 1;  
}

int countOnes ( const int * array, const int arrayLength ) {
    int count = 0;
    for ( int i = 0; i < arrayLength; i++ )
        if ( array[i] == 1 ) count++;
    return count;
}

long count;

int * array;
long arrayLength;

pthread_t * threadIDs;
int threadCount;

pthread_mutex_t lock;


void * threadCountOnes ( void * threadID ) {
    long ID = (long) threadID;
    double parts = (double)arrayLength/threadCount;
    long start = ID*parts;
    long end = start + parts;

    for ( int i = start; i < end; i++ ) {
        if ( array[i] == 1 ) {
            pthread_mutex_lock(&lock);
            count++;
            pthread_mutex_unlock(&lock);
        }
    }
    pthread_exit(NULL);
}


int main() {

    int arrayLengths[] = {1000, 1000000, 1000000000};
    int threadCounts[] = {1,2,4,32};

    for ( int lengthIndex = 0; lengthIndex < 3; lengthIndex++ ) {
        
        arrayLength = arrayLengths[lengthIndex];
        array = (int *) malloc(sizeof(int) * (long)arrayLength);
        fillArray(array, arrayLength);
        printf("Actual number of 1s: %i\n", countOnes(array,arrayLength));

        for ( int threadIndex = 0; threadIndex < 4; threadIndex++ ) {
            
            count = 0;
            pthread_mutex_init(&lock, NULL);

            threadCount = threadCounts[threadIndex];
            threadIDs = (pthread_t *) malloc((int)sizeof(pthread_t)*threadCount);

            clock_t start, end;
            double cpu_time_used;

            start = clock();

            for ( long i = 0; i < threadCount; i++ ) {
                pthread_create(&threadIDs[i],NULL,threadCountOnes,(void *)i);
            }

            for ( int i = 0; i < threadCount; i++ ) {
                pthread_join(threadIDs[i],NULL);
            }
            pthread_mutex_destroy(&lock);

            end = clock();
            cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

            printf("%li, %i, %li, %f\n",arrayLength, threadCount, count, cpu_time_used);
            free(threadIDs);
        
        } 

        free(array);
        printf("----------------\n");
    }

    pthread_exit(NULL);
    return 0;
}
