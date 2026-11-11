#include <stdio.h>
#include <stdbool.h>

#define P 5 // Number of processes
#define R 3 // Number of resources

void calculate_need(int need[P][R], int maxm[P][R], int allot[P][R]) {
    for (int i = 0 ; i < P ; i++)
        for (int j = 0 ; j < R ; j++)
            need[i][j] = maxm[i][j] - allot[i][j]; // FR5: Need = Maximum - Allocation
}

bool is_safe(int processes[], int avail[], int maxm[][R], int allot[][R], bool show_output) {
    int need[P][R];
    calculate_need(need, maxm, allot);

    bool finish[P] = {0};
    int safeSeq[P];
    int work[R];
    
    for (int i = 0; i < R; i++) work[i] = avail[i];

    int count = 0;
    while (count < P) {
        bool found = false;
        for (int p = 0; p < P; p++) {
            if (finish[p] == 0) {
                int j;
                for (j = 0; j < R; j++)
                    if (need[p][j] > work[j])
                        break;

                if (j == R) {
                    for (int k = 0 ; k < R ; k++)
                        work[k] += allot[p][k];
                    safeSeq[count++] = p;
                    finish[p] = 1;
                    found = true;
                }
            }
        }
        if (found == false) {
            if (show_output) printf("\nSystem is in an UNSAFE state! Deadlock is possible.\n");
            return false;
        }
    }

    if (show_output) {
        printf("\nSystem is in a SAFE state.\nSafe Sequence is: ");
        for (int i = 0; i < P ; i++) printf("P%d ", safeSeq[i]);
        printf("\n");
    }
    return true;
}

void run_bankers() {
    int processes[] = {0, 1, 2, 3, 4};
    
    // Test Case 1: Safe State Demonstration
    printf("\n--- Banker's Algorithm: Testing SAFE State ---\n");
    int avail_safe[R] = {3, 3, 2};
    int maxm_safe[P][R] = {{7, 5, 3}, {3, 2, 2}, {9, 0, 2}, {2, 2, 2}, {4, 3, 3}};
    int allot_safe[P][R] = {{0, 1, 0}, {2, 0, 0}, {3, 0, 2}, {2, 1, 1}, {0, 0, 2}};
    is_safe(processes, avail_safe, maxm_safe, allot_safe, true);

    // Test Case 2: Unsafe State Demonstration
    printf("\n--- Banker's Algorithm: Testing UNSAFE State ---\n");
    int avail_unsafe[R] = {3, 1, 1}; // Reduced available resources to force unsafe state
    int maxm_unsafe[P][R] = {{7, 5, 3}, {3, 2, 2}, {9, 0, 2}, {2, 2, 2}, {4, 3, 3}};
    int allot_unsafe[P][R] = {{0, 1, 0}, {2, 0, 0}, {3, 0, 2}, {2, 1, 1}, {0, 0, 2}};
    is_safe(processes, avail_unsafe, maxm_unsafe, allot_unsafe, true);
}

