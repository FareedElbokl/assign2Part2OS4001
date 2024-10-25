#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>

int main() {
    key_t key = 1234; // Shared memory key, used so processes can use the same key to share memory
    int shmid;
    int *shared_var;

    // Create a shared memory segment
    shmid = shmget(key, sizeof(int), IPC_CREAT | 0666); // IPC_CREAT flag specifies to create a shared mem segment if the key passed in does not already have an associated id
    if (shmid < 0) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    // Attach the shared memory segment to the process's address space
    shared_var = (int *)shmat(shmid, NULL, 0); // We type cast to an int pointer since shmat returns a void pointer, and C does not allow for implicit conversion from void * to int *
    if (shared_var == (int *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    // Seed the random number generator
    srand(time(NULL));

    pid_t pid = -1; // Initialize the process ID variable
    *shared_var = -1; // Initialize the shared variable

    while (1) {
        // Generate a random number between 0 and 10
        int random_value = rand() % 11;
        *shared_var = random_value; // Update the shared memory variable
        printf("I am Process 1. Generated value: %d\n", *shared_var);

        // If the shared variable is 9, start Process 2 (if we have not already)
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

        // If the shared variable is 0, exit
        if (*shared_var == 0 && pid != -1) {
            printf("Process 1 is exiting because the shared variable is 0.\n");
            if (pid > 0) {
                // Wait for Process 2 to finish if it was started
                waitpid(pid, NULL, 0);
            }
            break;
        }

        // Delay for a while to slow down the display speed
        sleep(1);
    }

    // Detach and remove the shared memory segment
    shmdt(shared_var);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
