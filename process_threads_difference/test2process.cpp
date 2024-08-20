#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char* argv[]) {
    int x = 2;
    int pid = fork();
    if (pid == -1){
        return 1;
    }

    if (pid == 0){
        x++;
    }
    sleep(2);
    std::cout << "Value of x:" << x << std::endl;

    std::cout << "Process id:" << getpid() << std::endl;
    if (pid != 0){
        wait(NULL);
    }
    return EXIT_SUCCESS;
}