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

struct ThreadData{
    std::string sourceFile;            //contains source address
    std::string destinationFile;       //contains destination address
    std::ifstream* inputFile;
    std::ofstream* outputFile;
    std::queue<std::string>* readData; //pointer to queue
    pthread_mutex_t* queueMutex;       //pointer to mutex
    pthread_cond_t* condQueueRead;
    pthread_cond_t* condQueueWrite;
    bool readComplete = false;
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
    pthread_cond_t condQueueRead;
    pthread_cond_t condQueueWrite;

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

    std::ifstream inputFile(copySource);
    std::ofstream outputFile(copyDestination);

    if(!inputFile.is_open()){
        std::cerr << "Error: Could not open file " << copySource << std::endl;
    }

    pthread_cond_init(&condQueueRead, NULL);
    pthread_cond_init(&condQueueWrite, NULL);
    pthread_mutex_init(&queueMutex, NULL);

    ThreadData* threadData = new ThreadData;

    threadData->sourceFile = copySource;
    threadData->destinationFile = copyDestination;
    threadData->readData = &string_queue;
    threadData->queueMutex = &queueMutex;
    threadData->condQueueRead = &condQueueRead;
    threadData->condQueueWrite = &condQueueWrite;
    threadData->inputFile = &inputFile;
    threadData->outputFile = &outputFile;



    std::vector<pthread_t> readThreads;
    std::vector<pthread_t> writeThreads;

    std::cout << threadNum << std::endl;

    for (int i = 0; i < threadNum; i++){
        pthread_t thread;

        if(pthread_create(&thread, NULL, read_call, threadData) != 0){
            std::cout << "read thread creation failed" << std::endl; 
        } else {
            readThreads.push_back(thread);
        }
    }

    for (int i = 0; i < threadNum; i++){
        pthread_t thread;

        if(pthread_create(&thread, NULL, write_call, threadData) != 0){
            std::cout << "write thread creation failed" << std::endl;
        } else {
            writeThreads.push_back(thread);
        }
    }

    std::cout << "made it out" << std::endl;

    for(pthread_t& thread : readThreads){
        pthread_join(thread, NULL);
    }
    for(pthread_t& thread : writeThreads){
        pthread_join(thread, NULL);
    }

    pthread_mutex_destroy(threadData->queueMutex);
    pthread_cond_destroy(threadData->condQueueRead);
    pthread_cond_destroy(threadData->condQueueWrite);

    return EXIT_SUCCESS;
}

void* read_call(void* args){
    ThreadData* threadData = static_cast<ThreadData*>(args);
    int queueFull = 20;
    std::string line;
    bool readingData = true;

    while(readingData){   
        pthread_mutex_lock(threadData->queueMutex);

        // reading validate conditions
        while (threadData->readData->size() >= queueFull){
            std::cout << "queue full, waiting..." << std::endl;
            pthread_cond_broadcast(threadData->condQueueWrite);
            pthread_cond_wait(threadData->condQueueRead, threadData->queueMutex);
        }
        if (threadData->inputFile->eof()){
            std::cout << "Reader end" << std::endl;
            pthread_cond_broadcast(threadData->condQueueWrite);
            threadData->readComplete = true;
            readingData = false;

        } else if (readingData){
            // reading process
            std::getline(*threadData->inputFile, line);
            threadData->readData->push(line);
            std::cout << "reading: " << line << std::endl;

        }
        pthread_mutex_unlock(threadData->queueMutex);
        pthread_cond_signal(threadData->condQueueWrite);

    }
    std::cout << "reader thread leaving" << std::endl;
    pthread_exit(NULL);
    return NULL;
};

void* write_call(void* args){
    ThreadData* threadData = static_cast<ThreadData*>(args);
    bool writingData = true;

    while(writingData){
        pthread_mutex_lock(threadData->queueMutex); // Lock then check

        while (threadData->readData->empty() && !threadData->readComplete){
            std::cout << "Queue is empty, waiting..." << std::endl;
            pthread_cond_broadcast(threadData->condQueueRead);
            pthread_cond_wait(threadData->condQueueWrite, threadData->queueMutex);
        }

        if(threadData->readComplete && threadData->readData->empty()){
                std::cout << "Writer end" << std::endl;
                writingData = false;
        } else {
            *threadData->outputFile << threadData->readData->front() << std::endl;
            std::cout << "writing: " << threadData->readData->front() << std::endl;
            threadData->readData->pop();
        }

        pthread_mutex_unlock(threadData->queueMutex);
        pthread_cond_signal(threadData->condQueueRead);
    }
    std::cout << "write thread lost" << std::endl;
    pthread_exit(NULL);
    return NULL;
};
