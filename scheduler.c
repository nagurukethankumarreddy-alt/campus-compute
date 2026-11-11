#include <stdio.h>
#include <stdlib.h>

// FR1: Represent jobs with required fields
typedef struct {
    int id;
    int arrival;
    int burst;
    int priority;
    int remaining_burst;
    int ct; // Completion Time
    int tat; // Turnaround Time
    int wt; // Waiting Time
    int status; // 0 = Pending, 1 = Completed
} Job;

void reset_jobs(Job jobs[], Job original[], int n) {
    for (int i = 0; i < n; i++) {
        jobs[i] = original[i];
    }
}

void print_metrics(Job jobs[], int n) {
    float total_tat = 0, total_wt = 0;
    printf("\nID\tArr\tBurst\tPrio\t|\tCT\tTAT\tWT\n");
    printf("----------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t|\t%d\t%d\t%d\n", 
            jobs[i].id, jobs[i].arrival, jobs[i].burst, jobs[i].priority, 
            jobs[i].ct, jobs[i].tat, jobs[i].wt);
        total_tat += jobs[i].tat;
        total_wt += jobs[i].wt;
    }
    printf("Average Turnaround Time: %.2f\n", total_tat / n);
    printf("Average Waiting Time: %.2f\n", total_wt / n);
}

// FR2: First Come, First Served (FCFS)
void run_fcfs(Job jobs[], int n) {
    printf("\n--- FCFS Scheduling ---\n");
    printf("Execution Timeline: ");
    int current_time = 0;
    
    for (int i = 0; i < n; i++) {
        if (current_time < jobs[i].arrival) current_time = jobs[i].arrival;
        
        printf("[T=%d] Job%d ", current_time, jobs[i].id);
        current_time += jobs[i].burst;
        
        jobs[i].ct = current_time;
        jobs[i].tat = jobs[i].ct - jobs[i].arrival;
        jobs[i].wt = jobs[i].tat - jobs[i].burst;
        jobs[i].status = 1;
    }
    printf("[T=%d]\n", current_time);
    print_metrics(jobs, n);
}

// FR2: Round Robin (RR)
void run_rr(Job jobs[], int n, int quantum) {
    printf("\n--- Round Robin Scheduling ---\n");
    printf("Time Quantum: %d units\n", quantum);
    printf("Execution Timeline: ");
    
    int current_time = 0, completed = 0;
    
    while (completed < n) {
        int idle = 1;
        for (int i = 0; i < n; i++) {
            if (jobs[i].arrival <= current_time && jobs[i].remaining_burst > 0) {
                idle = 0;
                printf("[T=%d] Job%d ", current_time, jobs[i].id);
                
                int exec_time = (jobs[i].remaining_burst > quantum) ? quantum : jobs[i].remaining_burst;
                current_time += exec_time;
                jobs[i].remaining_burst -= exec_time;
                
                if (jobs[i].remaining_burst == 0) {
                    jobs[i].ct = current_time;
                    jobs[i].tat = jobs[i].ct - jobs[i].arrival;
                    jobs[i].wt = jobs[i].tat - jobs[i].burst;
                    jobs[i].status = 1;
                    completed++;
                }
            }
        }
        if (idle) {
            current_time++;
        }
    }
    printf("[T=%d]\n", current_time);
    print_metrics(jobs, n);
}

// FR2: Non-preemptive Priority Scheduling
void run_priority(Job jobs[], int n) {
    printf("\n--- Priority Scheduling (Non-preemptive) ---\n");
    printf("Rule: Lower number = Higher priority. Tie-breaker: Lower Job ID executes first.\n");
    printf("Execution Timeline: ");
    
    int current_time = 0, completed = 0;
    
    while (completed < n) {
        int highest_prio_idx = -1;
        int highest_prio = 9999; 
        
        for (int i = 0; i < n; i++) {
            if (jobs[i].arrival <= current_time && jobs[i].status == 0) {
                // Tie-breaking rule implemented here (FR2)
                if (jobs[i].priority < highest_prio || 
                   (jobs[i].priority == highest_prio && jobs[i].id < jobs[highest_prio_idx].id)) {
                    highest_prio = jobs[i].priority;
                    highest_prio_idx = i;
                }
            }
        }
        
        if (highest_prio_idx != -1) {
            printf("[T=%d] Job%d ", current_time, jobs[highest_prio_idx].id);
            current_time += jobs[highest_prio_idx].burst;
            jobs[highest_prio_idx].ct = current_time;
            jobs[highest_prio_idx].tat = jobs[highest_prio_idx].ct - jobs[highest_prio_idx].arrival;
            jobs[highest_prio_idx].wt = jobs[highest_prio_idx].tat - jobs[highest_prio_idx].burst;
            jobs[highest_prio_idx].status = 1;
            completed++;
        } else {
            current_time++;
        }
    }
    printf("[T=%d]\n", current_time);
    print_metrics(jobs, n);
}

void run_scheduling() {
    int n = 4;
    Job original_jobs[] = {
        {1, 0, 8, 2, 8, 0, 0, 0, 0},
        {2, 1, 4, 1, 4, 0, 0, 0, 0},
        {3, 2, 9, 3, 9, 0, 0, 0, 0},
        {4, 3, 5, 2, 5, 0, 0, 0, 0} // Priority tie with Job 1 to demonstrate tie-breaking
    };
    
    Job jobs[4];

    // FCFS
    reset_jobs(jobs, original_jobs, n);
    run_fcfs(jobs, n);

    // Round Robin
    reset_jobs(jobs, original_jobs, n);
    run_rr(jobs, n, 4); // Quantum of 4

    // Priority
    reset_jobs(jobs, original_jobs, n);
    run_priority(jobs, n);
}
