#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define QUEUE_SIZE 5
#define NUM_ITEMS 15

// Shared bounded queue resources
int queue[QUEUE_SIZE];
int in = 0, out = 0, count = 0;

// POSIX synchronization variables
pthread_mutex_t mutex;
pthread_cond_t cond_full;
pthread_cond_t cond_empty;

FILE *log_file;

void* producer(void* arg) {
    for (int i = 1; i <= NUM_ITEMS; i++) {
        pthread_mutex_lock(&mutex);
        
        // Wait if the queue is full
        while (count == QUEUE_SIZE) {
            pthread_cond_wait(&cond_empty, &mutex);
        }
        
        // Produce item
        queue[in] = i;
        printf("[Producer] Generated Job Event %d\n", i);
        if (log_file) fprintf(log_file, "[Producer] Generated Job Event %d\n", i);
        
        in = (in + 1) % QUEUE_SIZE;
        count++;
        
        // Signal consumer that queue is no longer empty
        pthread_cond_signal(&cond_full);
        pthread_mutex_unlock(&mutex);
        
        usleep(100000); // 100ms delay to simulate job generation
    }
    return NULL;
}

void* consumer(void* arg) {
    for (int i = 1; i <= NUM_ITEMS; i++) {
        pthread_mutex_lock(&mutex);
        
        // Wait if the queue is empty
        while (count == 0) {
            pthread_cond_wait(&cond_full, &mutex);
        }
        
        // Consume item
        int item = queue[out];
        printf("[Consumer] Processed Job Event %d\n", item);
        if (log_file) fprintf(log_file, "[Consumer] Processed Job Event %d\n", item);
        
        out = (out + 1) % QUEUE_SIZE;
        count--;
        
        // Signal producer that queue is no longer full
        pthread_cond_signal(&cond_empty);
        pthread_mutex_unlock(&mutex);
        
        usleep(150000); // 150ms delay to simulate processing time
    }
    return NULL;
}

void run_logger() {
    printf("Initializing POSIX threads and synchronization objects...\n");
    
    // Open log file to generate readable event logs (FR3)
    log_file = fopen("log.txt", "w");
    if (!log_file) {
        perror("Error creating log file");
        return;
    }

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_full, NULL);
    pthread_cond_init(&cond_empty, NULL);

    pthread_t prod_thread, cons_thread;

    // Create concurrent threads
    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    // Join threads for orderly termination
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    // Cleanup IPC resources
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_full);
    pthread_cond_destroy(&cond_empty);
    fclose(log_file);
    
    printf("\n--- Thread Execution Complete ---\n");
    printf("Mutex and condition variables destroyed. Threads terminated cleanly.\n");
    printf("Activity logs have been written to 'log.txt'.\n");
}
