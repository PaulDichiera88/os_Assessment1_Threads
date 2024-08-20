#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

void* routine(void* arg);

int main(int argc, char* argv[]){
    pthread_t t1, t2; //variable type pthread

    // the if statement is in place because these pthread function return an int
    //if we find the function returning anything other than 0 then there may be a problem
    // the if statement gives us a simple error check

    
    if (pthread_create(&t1, NULL, &routine, NULL) != 0){ 
        return 1;
    }
    if (pthread_create(&t2, NULL, &routine, NULL) != 0){
        return 2;
    };
    if (pthread_join(t1, NULL) != 0){
        return 3;
    };
    if (pthread_join(t2, NULL) != 0){
        return 4;
    };

    return EXIT_SUCCESS;
}

void* routine(void* arg) {
    std::cout << "Test for Threads" << std::endl;
    sleep(3);
    std::cout << "Ending Thread" << std::endl;

    return NULL;
}