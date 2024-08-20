#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
void* routine(void* arg);
void* routine2(void* arg);

int main(int argc, char* argv[]){
    int x = 0;
    pthread_t t1,t2;
    pthread_create(&t1, NULL, &routine, &x);
    pthread_create(&t2, NULL, &routine2, &x);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    std::cout << "Final value of x: " << x << std::endl;


    return EXIT_SUCCESS;
}

void* routine(void* arg){
    int* x = (int*)arg;
    std::cout << "Process id:" << getpid() << std::endl;
    std::cout << "Value of x: " << *x << std::endl;
    (*x)++;
    sleep(2);
    return NULL;
}

void* routine2(void* arg){
    int* x = (int*)arg;
    std::cout << "Process id:" << getpid() << std::endl;
    std::cout << "Value of x: " << *x << std::endl;
    (*x)++;
    sleep(2);
    return NULL;
}
