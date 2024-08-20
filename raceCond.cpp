#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

void* routine(void* arg){
    int* mail = (int*) arg;
    (*mail)++;
    return NULL;
}

int main(int argc, char* argv[]){
    pthread_t p1, p2;
    int mail = 0;
    pthread_create(&p1, NULL, &routine, &mail;
    pthread_create(&p2, NULL, &routine, &mail);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    std::cout << "working" << std::endl;

    return EXIT_SUCCESS;
}