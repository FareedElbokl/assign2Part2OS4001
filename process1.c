#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <time.h>

void sem_lock(int semid);
void sem_unlock(int semid);

int main() {
    key_t key = 1234; // Shared memory key
    key_t sem_key = 5678; // Semaphore key
    int shmid, semid;
    int *shared_var;

    // Create a shared memory segment
    shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    // Attach the shared memory segment to the process's address space
    shared_var = (int *)shmat(shmid, NULL, 0);
    if (shared_var == (int *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    // Create a semaphore set with 1 semaphore
    semid = semget(sem_key, 1, IPC_CREAT | 0666);
    if (semid < 0) {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }

    // Initialize the semaphore to 1
    if (semctl(semid, 0, SETVAL, 1) < 0) {
        perror("semctl failed");
        exit(EXIT_FAILURE);
    }

    // Seed the random number generator
    srand(time(NULL));

    pid_t pid = -1; // Initialize the process ID variable
    *shared_var = -1; // Initialize the shared variable

    while (1) {
        // Generate a random number between 0 and 10
        int random_value = rand() % 11;

        // Lock the semaphore to protect shared memory access
        sem_lock(semid);
        *shared_var = random_value; // Update the shared memory variable
        printf("I am Process 1. Generated value: %d\n", *shared_var);
        sem_unlock(semid); // Unlock the semaphore

        // If the shared variable is 9, start Process 2
        if (*shared_var == 9 && pid == -1) {
            printf("Starting Process 2...\n");
            pid = fork();
            if (pid == 0) {
                // Child process: execute a separate program for Process 2
                execl("./process2", "process2", (char *)NULL);
                // If execl returns, it means there was an error
                perror("Failed to start Process 2");
                exit(EXIT_FAILURE);
            } else if (pid < 0) {
                // Fork failed
                perror("Fork failed");
                exit(EXIT_FAILURE);
            }
        }

        // Lock the semaphore before checking the shared variable
        sem_lock(semid);
        if (*shared_var == 0 && pid != -1) {
            printf("Process 1 is exiting because the shared variable is 0.\n");
            sem_unlock(semid); // Unlock before exiting
            if (pid > 0) {
                // Wait for Process 2 to finish if it was started
                waitpid(pid, NULL, 0);
            }
            break;
        }
        sem_unlock(semid); // Unlock the semaphore

        // Delay for a while to slow down the display speed
        sleep(1);
    }

    // Detach and remove the shared memory segment
    shmdt(shared_var);
    shmctl(shmid, IPC_RMID, NULL);

    // Remove the semaphore set
    semctl(semid, 0, IPC_RMID);

    return 0;
}

// Function to lock the semaphore (decrement)
void sem_lock(int semid) {
    struct sembuf sb = {0, -1, 0};
    semop(semid, &sb, 1);
}

// Function to unlock the semaphore (increment)
void sem_unlock(int semid) {
    struct sembuf sb = {0, 1, 0};
    semop(semid, &sb, 1);
}
