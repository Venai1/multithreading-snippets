#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define NUM_THREADS 7

// clear && gcc -o Question2a Question2a.c -pthread && ./Question2a

// Global variables
sem_t* semaphores[NUM_THREADS];
const int print_order[NUM_THREADS] = {0, 3, 1, 2, 6, 5, 4};

// Function to find the next thread in sequence
int get_next_thread(int current_thread) {
    for (int i = 0; i < NUM_THREADS; i++) {
        if (print_order[i] == current_thread) {
            if (i + 1 < NUM_THREADS) {
                return print_order[i + 1];
            }
        }
    }
    return -1;  // No next thread
}

// Thread function
void* print_hello(void* arg) {
    int thread_id = *(int*)arg;
    
    // Wait for your turn
    sem_wait(semaphores[thread_id]);
    
    // Print message
    printf("Hello from thread %d!\n", thread_id+1);
    
    // Signal the next thread in sequence
    int next_thread = get_next_thread(thread_id);
    if (next_thread != -1) {
        sem_post(semaphores[next_thread]);
    }
    
    free(arg);  // Free the allocated thread ID
    return NULL;
}

// Initialize semaphores
void init_semaphores() {
    for (int i = 0; i < NUM_THREADS; i++) {
        const char name = (char)i;
        semaphores[i] = sem_open(&name, O_CREAT, 0644, 0);
        if (semaphores[i] == SEM_FAILED) {
            printf("Failed to create semaphore %d: %s\n", i, strerror(errno));
            exit(1);
        }
        
    }
}

// Clean up semaphores
void cleanup_semaphores() {
    for (int i = 0; i < NUM_THREADS; i++) {
        sem_close(semaphores[i]);
    }
}

int main() {
    pthread_t threads[NUM_THREADS];
    
    // Initialize semaphores
    init_semaphores();
    
    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        int* tid = malloc(sizeof(int));
        *tid = i;
        if (pthread_create(&threads[i], NULL, print_hello, tid) != 0) {
            printf("Failed to create thread %d\n", i);
            cleanup_semaphores();
            exit(1);
        }
    }
    
    // Start the sequence by signaling the first thread in print_order
    sem_post(semaphores[print_order[0]]);
    
    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("All threads completed!\n");
    
    // Clean up
    cleanup_semaphores();
    
    return 0;
}