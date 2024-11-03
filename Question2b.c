#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

// clear && gcc -o Question2b Question2b.c -pthread && ./Question2b

int count = 0;
int NUM_THREADS = 7;
pthread_mutex_t count_mutex;
pthread_cond_t count_threshold_cv;

void* inc_count(void* arg) {
    int thread_id = *(int*)arg;
    
    printf("Hello from thread %d!\n", thread_id+1);
   
    pthread_mutex_lock(&count_mutex);
    count++;

    if (count == 2) {
        pthread_cond_broadcast(&count_threshold_cv);
        // printf("Sent signal from thread %d\n", thread_id+1);
        printf("=========\n");
    }

    pthread_mutex_unlock(&count_mutex);

    free(arg); 
    pthread_exit(NULL);
}

void* watch_count(void* arg) {
    int thread_id = *(int*)arg;
    
    pthread_mutex_lock(&count_mutex);
    
    while(count < 2) {
        pthread_cond_wait(&count_threshold_cv, &count_mutex);
    }
    
    printf("Hello from thread %d!\n", thread_id+1);
   
    pthread_mutex_unlock(&count_mutex);
    free(arg);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;
    
    pthread_mutex_init(&count_mutex, NULL);
    pthread_cond_init(&count_threshold_cv, NULL);
    
    for(int i = 0; i < NUM_THREADS; i++){
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        
        int* tid = malloc(sizeof(int));
        *tid = i;

        if(i == 4-1 || i == 1-1) {
            pthread_create(&threads[i], &attr, inc_count, tid);
        } else {
            if (i != 2-1) pthread_create(&threads[i], &attr, watch_count, tid);
        }
        
        pthread_attr_destroy(&attr);
        
    }
    
    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&count_mutex);
    pthread_cond_destroy(&count_threshold_cv);

    printf("All threads completed!\n");
    
    return 0;
}