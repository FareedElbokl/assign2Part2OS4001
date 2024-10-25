#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> // Provides types such as 'pid_t'
#include <time.h>

void delay(int seconds)
{
    sleep(seconds);
}

int main(){

    srand(time(NULL)); // Seed the randomizer with the current time

    pid_t pid;

    while (1){
        int random_int = rand() % 11; // Generate rand num between 0 and 10
        printf("I am Process 1. Generated int: %d\n", random_int);

        if (random_int > 5){
            printf("High value\n");
        } else {
            printf("Low value\n");
        }

        if (random_int == 9){
            // Start process 2
            pid = fork();

            if (pid < 0){
                perror("Unable to create process 2");
                exit(-1);
            } else if (pid == 0){
                // Child process 2

                // Execute the process2 seperate program
                execl("./process2", "process2", (char *)NULL);

                // If execl returns, then an error occured
                perror("Failed to start Process 2");
                exit(-1);
            }
        }

        delay(1); // Wait one second
    }

    return 0;
}