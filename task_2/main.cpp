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

void* read_call(void* arg);
void* write_call(void* arg);

struct copyDirectories{
    std::string sourceFile;
    std::string destinationFile;
};


// Threads that read data to a container, Threads that write data from that container
int main(int argc, char* argv[]){

    if (argc != 4){
        std::cout << "required input: <number of files to copy, integer> <source folder> <destination folder> " << std::endl;
    }

    int threadNum = std::stoi(argv[1]); //number of threads to use in operations
    std::string copySource = argv[2]; //save source file 
    std::string copyDestination = argv[3]; // save destination file

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

    copyDirectories* copyDir = new copyDirectories;
    copyDir->sourceFile = copySource;
    copyDir->destinationFile = copyDestination;

    std::vector<pthread_t> readThreads;
    std::vector<pthread_t> writeThreads;

    std::cout << threadNum << std::endl;

    for (int i = 0; i < threadNum; i++){
        pthread_t thread;

        if(pthread_create(&thread, NULL, read_call, copyDir) != 0){
            std::cout << "read thread creation failed" << std::endl; 
        } else {
            readThreads.push_back(thread);
        }
    }

    for (int i = 0; i < threadNum; i++){
        pthread_t thread;

        if(pthread_create(&thread, NULL, write_call, copyDir) != 0){
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

void* read_call(void* arg){
    std::cout << "read thread called" << std::endl;

    return NULL;
};

void* write_call(void* arg){
    std::cout << "write thread called" << std::endl;

    return NULL;
};