#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // OS API functions such as fork() and exec()
#include <sys/types.h> // Provides types such as 'pid_t'


void delay(int seconds){
    sleep(seconds); // sleep() in unix pauses execution for a certain amount of seconds
}
int main(){

    pid_t pid1, pid2;

    // Create first child process
    pid1 = fork();

    // If pid is < 0 then an error occured
    if (pid1 < 0){
        perror("Failed to fork process 1"); // Displays native error message along with custom one
        exit(1);
    } else if (pid1 == 0){
        // Child process 1
        while (1){
            printf("I am Process 1\n");
            fflush(stdout); // Forces an output to be written to standard output stream even if being buffered.
            delay(1); // Delay for one second
        }
    } else {
        // Parent process
        // Create second child process
        pid2 = fork();

        if (pid2 < 0){
            perror("Failed to fork process 2");
            exit(1);
        } else if (pid2 == 0){
            // Child process 2
            while (1){
                printf("I am Process 2\n");
                fflush(stdout); // Force output to be displayed
                delay(2); // Delay for two seconds
            }
        } else {
            // Parent process: wait indefinitely
            printf("Parent process: Use 'ps' to find PIDs and 'kill' to terminate the child processes.\n");
            while (1){
                // Parent process does nothing, waits for manual termination
                delay(5);
            }
        }
    }

    return 0;
}