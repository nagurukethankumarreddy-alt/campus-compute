#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>

#define SHM_NAME "/campus_shm"
#define SEM_NAME "/campus_sem"
#define SHM_SIZE 1024

void run_ipc_worker() {
    printf("\n--- Setting up IPC Resources ---\n");

    // FR4: Create POSIX Shared Memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Shared memory failed");
        return;
    }
    ftruncate(shm_fd, SHM_SIZE);
    char *shared_mem = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // FR4: Create Named Semaphore
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("Semaphore initialization failed");
        return;
    }

    printf("Shared memory and named semaphore created successfully.\n");

    // FR6: Worker 1 - Normal Completion
    printf("\n--- Starting Normal Worker Process ---\n");
    pid_t pid1 = fork();

    if (pid1 < 0) {
        perror("Fork failed");
    } else if (pid1 == 0) {
        // Child 1: Write to shared memory
        sem_wait(sem);
        sprintf(shared_mem, "Job completed successfully by Worker PID %d", getpid());
        printf("[Worker 1 (PID: %d)] Wrote data to shared memory. Exiting normally.\n", getpid());
        sem_post(sem);
        exit(0); // Normal completion
    } else {
        // Parent: Wait for Child 1
        int status;
        waitpid(pid1, &status, 0); // Collect status using waitpid
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("[Parent] Normal Worker (PID: %d) completed successfully.\n", pid1);
            sem_wait(sem);
            printf("[Parent] Read from IPC Shared Memory: '%s'\n", shared_mem);
            sem_post(sem);
        }
    }

    // FR6: Worker 2 - Simulated Worker Failure
    printf("\n--- Starting Simulated Failure Worker Process ---\n");
    pid_t pid2 = fork();

    if (pid2 < 0) {
        perror("Fork failed");
    } else if (pid2 == 0) {
        // Child 2: Simulate a crash/failure
        printf("[Worker 2 (PID: %d)] Encountered a simulated critical error! Terminating abnormally.\n", getpid());
        exit(1); // Non-zero exit represents failure
    } else {
        // Parent: Wait for Child 2 and detect failure
        int status;
        waitpid(pid2, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            printf("[Parent] Detected simulated failure in Worker (PID: %d). Exit code: %d\n", pid2, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("[Parent] Worker (PID: %d) was terminated by a signal.\n", pid2);
        }
    }

    // Clean up resources (FR4 & FR7)
    printf("\n--- Cleaning up IPC Resources ---\n");
    munmap(shared_mem, SHM_SIZE);
    close(shm_fd);
    shm_unlink(SHM_NAME);
    sem_close(sem);
    sem_unlink(SEM_NAME);
    printf("Shared memory and semaphores unlinked and cleaned up.\n");
}
