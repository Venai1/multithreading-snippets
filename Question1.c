#include <pthread.h> // All pthreads programs must include this!
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool isSorted = true;
int *arr1; 
int arrLength;

void* compareNext(void* tid);

int main(int argc, char** argv) {
    printf("Enter the number of elements in the array: ");
    scanf("%d", &arrLength);

    arr1 = (int*) malloc(arrLength * sizeof(int));

    printf("Enter %d elements:\n", arrLength);
    for (int i = 0; i < arrLength; i++) {
        scanf("%d", &arr1[i]);
    }

    // thread identifiers
    pthread_t workers[arrLength-1];
  
    // set of thread attributes
    pthread_attr_t attr[arrLength-1]; 

    for (int i = 0; i < arrLength-1; ++i) {
        printf("Creating thread %i\n", i);
        
        // set default attributes of thread       
        pthread_attr_init(&attr[i]); 

        // create the thread and tell it what function to execute
        int* tid = malloc(sizeof(int));  // Allocate memory for thread id
        *tid = i;
        pthread_create(&workers[i], &attr[i], compareNext, tid); 
    }

    // Master thread waits on each launched thread to finish
    for (int i = 0; i < arrLength-1; ++i) {
        pthread_join(workers[i], NULL);
        printf("Thread #%i joined!\n", i);
    }

    if (isSorted) {
        printf("====================\nThe array is sorted\n====================");
    } else {
        printf("====================\nThe array is not sorted\n====================");
    }
    
    return 0;
}

// Each thread executes this function
void* compareNext(void* tid) {
    int threadid = *(int*) tid;  // Dereference pointer to get the int value
    free(tid);  
    if (arr1[threadid] > arr1[threadid+1]){
      isSorted = false;
    }
    pthread_exit(0);
}
// 