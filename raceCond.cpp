#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

void* routine(void* arg){
    int* mail = (int*) arg;
    for (int i = 0; i < 200; i++){
        (*mail)++;
    }
    //sleep(5);
    return NULL;
}

int main(int argc, char* argv[]){
    pthread_t p1, p2;
    int mail = 0;
    pthread_create(&p1, NULL, &routine, &mail);
    pthread_create(&p2, NULL, &routine, &mail);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    std::cout << "Number of mails: " << mail << std::endl;

    std::cout << "working" << std::endl;

    return EXIT_SUCCESS;
}