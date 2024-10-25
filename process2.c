#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> // Provides types such as 'pid_t'

void delay(int seconds){
    sleep(seconds);
}

int main(){

    while (1){
        printf("I am process 2\n");
        fflush(stdout);
        delay(2);
    }

    return 0;
}