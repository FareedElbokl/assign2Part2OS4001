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
    // Semaphore: Essentially before a process does anything to shared memory it first locks (decrements) the semaphore, but if another process currently has the semaphore locked, then it waits until that process unlocks (increments) the semaphore
    key_t key = 1234;     // Shared memory key
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
    shared_var = (int *)shmat(shmid, NULL, 0); // pass in shmid, NULL to specify that we dont care where in the process' address space the shared mem is attached.
    if (shared_var == (int *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    // Create a semaphore set with 1 semaphore
    semid = semget(sem_key, 1, IPC_CREAT | 0666); // Pass in the sem key, number of sems, and sem flag
    if (semid < 0) {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }

    // Initialize the semaphore to 1
    if (semctl(semid, 0, SETVAL, 1) < 0) { // semctl used to perform initialization and deletion operations on semaphores
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
        sem_lock(semid); // This means process 1 has exclusive access to the shared memory, and other processes must wait until semaphore is unlocked
        *shared_var = random_value; // Update the shared memory variable
        printf("I am Process 1. Generated value: %d\n", *shared_var);
        sem_unlock(semid); // Unlock the semaphore

        // If the shared variable is 9, start Process 2, assuming we have not yet started it already
        if (*shared_var == 9 && pid == -1) {
            printf("Starting Process 2...\n");
            pid = fork();
            if (pid == 0) {
                // Child process: execute a separate program for Process 2
                execl("./Part_II_101256495_101261926_program2", "Part_II_101256495_101261926_program2", (char *)NULL);
                // If execl returns, it means there was an error
                perror("Failed to start Process 2");
                exit(EXIT_FAILURE);
            } else if (pid < 0) {
                // Fork failed
                perror("Fork failed");
                exit(EXIT_FAILURE);
            }
        }

        // Lock the semaphore before checking the shared variable, to ensure the shared var is not modified
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

    // Detach the shared mem from the process' address space and remove the shared mem segment
    shmdt(shared_var);
    shmctl(shmid, IPC_RMID, NULL);

    // Remove the semaphore set
    semctl(semid, 0, IPC_RMID);

    return 0;
}

// Function to lock the semaphore (decrement)
void sem_lock(int semid) {
    struct sembuf sb = {0, -1, 0}; // Middle value specifies the semaphore operation to be done, here we decrement to 0 as we want to lock the semaphore
    semop(semid, &sb, 1);
}

// Function to unlock the semaphore (increment)
void sem_unlock(int semid) {
    struct sembuf sb = {0, 1, 0}; // {semnum in semaphore set, sem op, sem flag}
    semop(semid, &sb, 1);
}
