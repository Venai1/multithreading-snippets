#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int counter = 0;
int odd_even = 0;
pthread_mutex_t mutex;
pthread_cond_t cond_var;

int *arr1; 
int arrLength;

bool isSorted() {
    for (int i = 0; i < arrLength - 1; i++) {
        if (arr1[i] > arr1[i + 1]) {
            return false;
        }
    }
    return true;
}

void* even_odd_sort(void* arg) {
    int threadID = *(int*)arg;
    printf("DEBUG: Thread %d started\n", threadID);
    
    while (true) {
        if (isSorted()) {
            pthread_cond_broadcast(&cond_var);
            break;
        }
        int left = threadID + odd_even;
        int right = left + 1;

        if (right < arrLength) {
            if (arr1[left] > arr1[right]) {
                int temp = arr1[left];
                arr1[left] = arr1[right];
                arr1[right] = temp;
            }
        }

        // Barrier
        //printf("DEBUG: Locking %d mutex\n", threadID);
        pthread_mutex_lock(&mutex);

        counter++;
        if (counter == arrLength / 2) {
            //printf("DEBUG: Thread %d sending broadcast\n", threadID);
            counter = 0;
            odd_even = (odd_even+1) % 2;
            pthread_cond_broadcast(&cond_var);
        }

        // This check is done under the lock
        if (counter < arrLength / 2) {
            //printf("DEBUG: Thread %d waiting\n", threadID);
            pthread_cond_wait(&cond_var, &mutex);
            //printf("DEBUG: Thread %d FREE\n", threadID);
        }

        //printf("DEBUG: Unlocking %d mutex\n", threadID);
        pthread_mutex_unlock(&mutex);

    
    }
    free(arg);  // Free allocated memory for thread ID
    return NULL;
}

int main() {
    printf("Enter the number of elements in the array: ");
    while (scanf("%d", &arrLength) != 1 || arrLength <= 0) {
        // Clear the input buffer
        while (getchar() != '\n'); // Discard invalid input
        printf("Invalid input. Please enter a positive integer: ");
    }

    arr1 = (int*)malloc(arrLength * sizeof(int));

    printf("Enter %d elements:\n", arrLength);
    for (int i = 0; i < arrLength; i++) {
        while (scanf("%d", &arr1[i]) != 1) {
            // Clear the input buffer
            while (getchar() != '\n'); // Discard invalid input
            printf("Invalid input. Please enter an integer for element %d: ", i + 1);
        }
    }

    int threadCount = (arrLength + 1) / 2; // Number of threads for even indices
    pthread_t threads[threadCount];
    pthread_attr_t attr;

    for (int i = 0, tid = 0; i < arrLength-1; i += 2, tid++) {
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        
        int* threadID = malloc(sizeof(int));  // Allocate memory for thread argument
        *threadID = i;  // Assign even index

        pthread_create(&threads[tid], &attr, even_odd_sort, threadID);
        pthread_attr_destroy(&attr);  // Clean up attribute
    }

    // Wait for all threads to complete
    for (int i = 0; i < threadCount; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads completed!\n");

    printf("Final Sorted Array is: \n===================\n");

    for(int i = 0; i < arrLength; i++) {
        printf("%d ", arr1[i]);
    }

     printf("\n===================\n");

    // Clean up
    free(arr1); // Don't forget to free the allocated array
    return 0;
}
