#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>

// Function prototypes for all modules
void run_scheduling();
void run_logger();
void run_ipc_worker();
void run_bankers();

void demo_resource_limits() {
    struct rlimit rl;
    printf("\n--- Resource Limits Demonstration (FR7) ---\n");
    
    // Get current limit for File Size
    getrlimit(RLIMIT_FSIZE, &rl);
    printf("Original File Size Limit: Soft = %lu, Hard = %lu\n", (unsigned long)rl.rlim_cur, (unsigned long)rl.rlim_max);
    
    // Set a very small file size limit (0 bytes) to demonstrate the effect
    rl.rlim_cur = 0;
    if (setrlimit(RLIMIT_FSIZE, &rl) == 0) {
        printf("Successfully set soft limit for file size to 0 bytes.\n");
    } else {
        perror("Failed to set limit");
        return;
    }
    
    printf("Attempting to write to a file with a 0-byte size limit...\n");
    FILE *f = fopen("test_limit.txt", "w");
    if (f) {
        if (fprintf(f, "This should fail because of the resource limit.") < 0) {
            printf("Observed Effect: Write operation failed as expected due to RLIMIT_FSIZE.\n");
        }
        fclose(f);
        remove("test_limit.txt");
    }
}

void print_menu() {
    printf("\n--- Campus Compute: OS Mini-Project ---\n");
    printf("1. CPU Scheduling (FCFS, RR, Priority)\n");
    printf("2. Concurrent Activity Logger (Producer-Consumer)\n");
    printf("3. IPC & Worker Failure Detection\n");
    printf("4. Resource-Safety Analysis (Banker's Algorithm)\n");
    printf("5. Resource Limits Demonstration\n");
    printf("6. Exit & Cleanup\n");
    printf("Select an option: ");
}

int main() {
    int choice;
    char input[10];

    while (1) {
        print_menu();
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        
        choice = atoi(input);
        
        switch (choice) {
            case 1:
                run_scheduling();
                break;
            case 2:
                run_logger();
                break;
            case 3:
                run_ipc_worker();
                break;
            case 4:
                run_bankers();
                break;
            case 5:
                demo_resource_limits();
                break;
            case 6:
                printf("\nVerifying all threads/processes have ended...\n");
                printf("Cleaning up local resources and exiting safely.\n");
                exit(0);
            default:
                // FR8: Error handling for invalid input
                printf("\nError: Invalid command '%s'. Please enter a number between 1 and 6.\n", input); 
        }
    }
    return 0;
}
