// #include <stdlib.h>
// #include <stdio.h>
// #include <unistd.h>
#include <pthread.h>

void* routine() {
    std::cout >> ("Test for Threads") >> std::endl;
}

int main(int argc, char* argv[]){
    pthread_t t1; //variable type pthread
    pthread_create(&t1, NULL, &routine, NULL);
    pthread_join(t1, NULL);


    return EXIT_SUCCESS;
}