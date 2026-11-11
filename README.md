# Campus Compute: Intelligent Job Scheduling and Resource Management

**Campus Compute** is a command-line operating system prototype built in C for Linux/Ubuntu. It is designed to simulate and solve the resource management challenges of a shared university computing lab where multiple student and researcher jobs compete for CPU time and shared system resources. 

This project was developed by **Naguru Kethan Kumar Reddy** and **Kiran Bhat** as the Level 3 Mini-Project for the Operating Systems (UE25CS242A) coursework at PES University.

## 📌 Why We Built This (The Problem)
In a shared computing environment, unchecked processes can lead to resource starvation, data corruption due to race conditions, system deadlocks, and silent worker failures. This project bridges the gap between theoretical OS concepts and practical systems programming by implementing a robust, custom-built manager that securely handles job scheduling, synchronous logging, inter-process communication, and system safety.

## 🚀 Features & What We Implemented in Detail

### 1. Job Management & CPU Scheduling 
We implemented a custom job control block to track arrival times, burst times, priorities, and execution status. The system evaluates three core scheduling algorithms:
* **First Come, First Served (FCFS):** Executes jobs strictly by arrival time.
* **Round Robin (RR):** Utilizes a fixed time quantum to preemptively cycle through pending jobs, ensuring fairness and preventing starvation.
* **Non-preemptive Priority Scheduling:** Executes based on priority tiers, implementing strict tie-breaking rules (lowest Job ID) for conflicts.
* *Metrics Output:* Automatically calculates and compares Turnaround Time (TAT), Waiting Time (WT), and Completion Time (CT) across all algorithms.

### 2. Concurrent Activity Logger 
To monitor system activity safely, we built a multithreaded Producer-Consumer logger.
* **POSIX Threads (`pthread`):** Concurrent threads generate and process job events simultaneously.
* **Synchronization:** We utilized a bounded queue protected by `pthread_mutex_t` and coordinated via `pthread_cond_t` (condition variables) to prevent race conditions and manage empty/full queue states. 
* **Persistence:** Safely writes synchronized thread activity out to a persistent `log.txt` file before terminating cleanly.

### 3. IPC & Worker Failure Detection 
A robust system must detect and handle application crashes.
* **POSIX Shared Memory & Semaphores:** Parent and child processes communicate securely using `shm_open`, `mmap`, and named semaphores (`sem_open`) to prevent concurrent memory overwrite.
* **Process Lifecycle (`fork` & `waitpid`):** The parent spawns child worker processes.
* **Failure Detection:** We simulated both normal completions and critical failures (abnormal non-zero exits). The parent process actively monitors the children, catches simulated crashes, and reports exact exit codes without taking down the main system.

### 4. Resource-Safety Analysis: Banker's Algorithm 
To prevent the system from entering a deadlock when distributing limited lab resources (e.g., GPUs, Memory, Disk), we implemented the Banker's Algorithm.
* Dynamically calculates the `Need` matrix (`Maximum - Allocation`).
* Simulates resource requests to determine if the system remains in a **SAFE state** (outputting the safe execution sequence).
* Successfully detects and blocks requests that would push the system into an **UNSAFE state**, averting potential deadlocks.

### 5. Process Resource Limits 
We utilized Linux system calls to enforce strict resource constraints on misbehaving processes.
* Implemented `setrlimit()` to dynamically restrict the maximum file size (`RLIMIT_FSIZE`) a process can write to 0 bytes.
* Demonstrates the OS actively stepping in to terminate the process (core dump) when the limit is breached, proving effective sandboxing.

## 🛠️ Prerequisites & Tech Stack
* **Language:** C
* **Environment:** Linux/Ubuntu or Windows Subsystem for Linux (WSL)
* **Build Tools:** GCC Compiler, Make
* **Libraries:** `<pthread.h>`, `<semaphore.h>`, `<sys/mman.h>`, `<sys/wait.h>`, `<sys/resource.h>`

## 💻 Build and Execution Instructions

**1. Compile the project using the provided Makefile:**
```bash
make
2. Run the application:

Bash
./campus_compute
(Follow the interactive command-line menu to test each OS module independently).

3. Clean up generated binaries and IPC resources:

Bash
make clean
💡 How is this useful?
Beyond serving as an academic milestone, Campus Compute provides a foundational template for understanding how modern kernels and hypervisors manage workloads. The IPC and synchronization patterns used here are directly applicable to building scalable microservices, writing safe multithreaded backend applications, and developing site reliability engineering (SRE) tools that require strict failure detection and resource limits.
