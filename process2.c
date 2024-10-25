#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> // Provides types such as 'pid_t'

void delay(int seconds){
    sleep(seconds);
}

int main(){

    for (int i = 0; i < 10; i++) // Display message 10 times
    {
        printf("I am process 2\n"); // Display message
        fflush(stdout); // Force the output
        delay(1); // Delay for 1 second
    }

    return 0;
}