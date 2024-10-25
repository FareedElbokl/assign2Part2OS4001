#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
    key_t key = 1234; // Shared memory key
    int shmid;
    int *shared_var;

    // Access the shared memory segment
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

    // Loop until the shared variable is 0
    while (1) {
        printf("I am Process 2. Shared value: %d\n", *shared_var);

        // If the shared variable is 0, exit
        if (*shared_var == 0) {
            printf("Process 2 is exiting because the shared variable is 0.\n");
            break;
        }

        // Delay to slow down the display speed
        sleep(1);
    }

    // Detach the shared memory segment
    shmdt(shared_var);

    return 0;
}
