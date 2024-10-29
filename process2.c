#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

void sem_lock(int semid);
void sem_unlock(int semid);

int main() {
    key_t key = 1234; // Shared memory key
    key_t sem_key = 5678; // Semaphore key
    int shmid, semid;
    int *shared_var;

    // Access the shared memory segment that was created in process 1
    shmid = shmget(key, sizeof(int), 0666);
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

    // Access the semaphore set
    semid = semget(sem_key, 1, 0666);
    if (semid < 0) {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }

    // Loop until the shared variable is 0
    while (1) {
        sem_lock(semid); // Lock the semaphore to protect access
        printf("I am Process 2. Shared value: %d\n", *shared_var);
        if (*shared_var == 0) {
            printf("Process 2 is exiting because the shared variable is 0.\n");
            sem_unlock(semid); // Unlock before exiting
            break;
        }
        sem_unlock(semid); // Unlock the semaphore

        // Delay to slow down the display speed
        sleep(1);
    }

    // Detach the shared memory segment
    shmdt(shared_var);

    return 0;
}

// Function to lock the semaphore (decrement)
void sem_lock(int semid) {
    struct sembuf sb = {0, -1, 0}; // Specify that the operation is decrementing {semnum in semaphore set, sem op, sem flag}
    semop(semid, &sb, 1);
}

// Function to unlock the semaphore (increment)
void sem_unlock(int semid) {
    struct sembuf sb = {0, 1, 0}; // Specify that the operation is incrementing
    semop(semid, &sb, 1);
}
