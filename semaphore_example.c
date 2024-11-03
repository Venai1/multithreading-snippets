#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>    // For O_* constants
#include <errno.h>
#include <string.h>   // For strerror()

// Global semaphore pointer
sem_t* mutex;
const char* SEM_NAME = "/my_semaphore";

void* thread(void* arg) {
    printf("Thread %ld trying to enter critical section...\n", (long)pthread_self());
    
    // Wait
    sem_wait(mutex);
    printf("Thread %ld entered critical section\n", (long)pthread_self());
    
    // Critical section
    sleep(4);  // Simulates some work in the critical section
    
    // Signal
    printf("Thread %ld exiting critical section\n", (long)pthread_self());
    sem_post(mutex);
    
    return NULL;
}

int main() {
    printf("Starting semaphore example...\n");
    
    // First, try to unlink any existing semaphore with this name
    sem_unlink(SEM_NAME);
    
    // Create and initialize the named semaphore
    mutex = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if (mutex == SEM_FAILED) {
        printf("Semaphore initialization failed: %s\n", strerror(errno));
        return 1;
    }
    
    pthread_t t1, t2;
    
    // Create the first thread
    if (pthread_create(&t1, NULL, thread, NULL) != 0) {
        printf("Thread 1 creation failed\n");
        sem_close(mutex);
        sem_unlink(SEM_NAME);
        return 1;
    }
    
    sleep(2);  // Wait before creating the second thread
    
    // Create the second thread
    if (pthread_create(&t2, NULL, thread, NULL) != 0) {
        printf("Thread 2 creation failed\n");
        sem_close(mutex);
        sem_unlink(SEM_NAME);
        return 1;
    }
    
    // Wait for both threads to complete
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    // Clean up the semaphore
    sem_close(mutex);
    sem_unlink(SEM_NAME);
    
    printf("Program completed successfully\n");
    return 0;
}