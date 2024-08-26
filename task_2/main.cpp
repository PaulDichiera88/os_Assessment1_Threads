#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <string>
#include <filesystem>
#include <vector>
#include <dirent.h>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <ostream>
#include <unistd.h>
#include <queue>
#include <mutex>

void* read_call(void* arg);
void* write_call(void* arg);
void read_to_queue(const std::string& source);
void write_from_queue(const std::string& destination);
int lock_check(void* arg);

struct ThreadData{
    std::string sourceFile;            //contains source address
    std::string destinationFile;       //contains destination address
    std::queue<std::string>* fileData; //pointer to queue
    pthread_mutex_t* queueMutex;            //pointer to mutex
};


// Threads that read data to a container, Threads that write data from that container
int main(int argc, char* argv[]){

    if (argc != 4){
        std::cout << "required input: <number of files to copy, integer> <source folder> <destination folder> " << std::endl;
    }

    int threadNum = std::stoi(argv[1]); //number of threads to use in operations
    std::string copySource = argv[2]; //save source file 
    std::string copyDestination = argv[3]; // save destination file
    std::queue<std::string> string_queue;
    pthread_mutex_t queueMutex;

    if(std::filesystem::exists(copySource)){
        std::cout << "File location: Success. " << copySource << std::endl;
    } else {
        std::cout << "File location: Failed. " << "Source file is incorrect or does not exist" << std::endl;
    }

    if(std::filesystem::exists(copyDestination)){
        std::cout << "File location: Success. " << copyDestination << std::endl;
    } else {
        std::cout << "File location: Failed. " << "Destination file name is incorrect or does not exist" << std::endl;
        std::ofstream destinationFile(copyDestination);
        std::cout << "** A destination file has been created **" << std::endl;
    }

    pthread_mutex_init(&queueMutex, NULL);
    ThreadData* T_data = new ThreadData;
    T_data->sourceFile = copySource;
    T_data->destinationFile = copyDestination;
    T_data->fileData = &string_queue;
    T_data->queueMutex = &queueMutex;


    std::vector<pthread_t> readThreads;
    std::vector<pthread_t> writeThreads;

    std::cout << threadNum << std::endl;

    for (int i = 0; i < threadNum; i++){
        pthread_t thread;

        if(pthread_create(&thread, NULL, read_call, T_data) != 0){
            std::cout << "read thread creation failed" << std::endl; 
        } else {
            readThreads.push_back(thread);
        }
    }

    for (int i = 0; i < threadNum; i++){
        pthread_t thread;

        if(pthread_create(&thread, NULL, write_call, T_data) != 0){
            std::cout << "write thread creation failed" << std::endl;
        } else {
            writeThreads.push_back(thread);
        }
    }

    for(pthread_t& thread : readThreads){
        pthread_join(thread, NULL);
    }
    for(pthread_t& thread : writeThreads){
        pthread_join(thread, NULL);
    }

    // Shopping list:
    // a loop that can scale and organise thread creation .ie n = 10, 10 readers and 10 writer threads
        // for now i have created two loops i could create a single one that creates reads and writes one after another.
        // right now i am not sure what the better course of action is.
    // a possible data structure to hold the source and destination location
        // the answer to that is to you a queue and to limit its size to 20
    // a function to read facilitate reading and writiting from the source to the destination
        // I belive each read and write function will ask the lock if it is available. if it is not the thread would get put to sleep and 
        // woken up when it is its turn to read or write, i think having containers for reading and writing will help serve to organise 
        // my threads.
    // a mutex lock to control the threads use of memory
    //


    return EXIT_SUCCESS;
}

void* read_call(void* args){
    ThreadData* Thread_dirs = static_cast<ThreadData*>(args);
    pthread_mutex_lock(Thread_dirs->queueMutex);
    std::cout << "read thread called" << std::endl;
    read_to_queue(Thread_dirs->sourceFile);
    pthread_mutex_unlock(Thread_dirs->queueMutex);
    return NULL;
};

void* write_call(void* args){
    int lock = 0;
    ThreadData* Thread_dirs = static_cast<ThreadData*>(args);
    lock = lock_check(Thread_dirs);
    if (lock == 1){
    pthread_mutex_lock(Thread_dirs->queueMutex);
    std::cout << "write thread called" << std::endl;
    write_from_queue(Thread_dirs->destinationFile);
    }
    pthread_mutex_unlock(Thread_dirs->queueMutex);
    return NULL;
};

void read_to_queue(const std::string& source){

    //std::cout << source << std::endl;

};

void write_from_queue(const std::string& destination){

    //std::cout << destination << std::endl;

};

int lock_check(void* args){
    ThreadData* Thread_dirs = static_cast<ThreadData*>(args);
    std::cout << Thread_dirs->sourceFile << std::endl;

    //checks queue and decides to lock or unlock

    return 1;
};