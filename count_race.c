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

void * threadCountOnes ( void * threadID ) {
    long ID = (long) threadID;
    double parts = (double)arrayLength/threadCount;
    long start = ID*parts;
    long end = start + parts;

    for ( int i = start; i < end; i++ ) {
        if ( array[i] == 1 ) {
            count++;
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
        int properCount = countOnes(array,arrayLength);
        printf("Actual number of 1s: %i\n", properCount);

        for ( int threadIndex = 0; threadIndex < 4; threadIndex++ ) {
           

            threadCount = threadCounts[threadIndex];
            threadIDs = (pthread_t *) malloc((int)sizeof(pthread_t)*threadCount);

            long results[100];
            double times [100];

            clock_t start, end;
            double cpu_time_used;
            
            for ( int i = 0; i < 100; i++ ) { 
                count = 0;
                start = clock();

                for ( long i = 0; i < threadCount; i++ ) {
                    pthread_create(&threadIDs[i],NULL,threadCountOnes,(void *)i);
                }

                for ( int i = 0; i < threadCount; i++ ) {
                    pthread_join(threadIDs[i],NULL);
                }

                end = clock();
                cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

                results[i] = count;
                times[i] = cpu_time_used;
            }

            double averageCount = 0;
            double averageTime = 0;
            int timesCorrect = 0;
            for ( int i = 0; i < 100; i++ ) {
                averageCount += results[i];
                averageTime += times[i];
                if ( results[i] == properCount )
                    timesCorrect++;
            }
            averageTime /= 100;
            averageCount /= 100;


            printf("Array Length: %li, Thread Count: %i, Average Result: %f, Average Time: %f, Times Correct/100: %i\n",
                    arrayLength, threadCount, averageCount, averageTime, timesCorrect );
            free(threadIDs);
        
        } 

        free(array);
        printf("----------------\n");
    }

    pthread_exit(NULL);
    return 0;
}
