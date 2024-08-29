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
    std::queue<std::string>* readData; //pointer to queue
    pthread_mutex_t* queueMutex;       //pointer to mutex
    pthread_cond_t* condQueueRead;
    pthread_cond_t* condQueueWrite;
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
    int queueFull = 20;
    std::string line;
    bool readingData = true;

    std::cout << "Entered read_call " << std::endl; 

    ThreadData* threadData = static_cast<ThreadData*>(args);
    std::ifstream inputFile(threadData->sourceFile);

    if(!inputFile.is_open()){
        std::cerr << "Error: Could not open file " << threadData->sourceFile << std::endl;
    }

    while(readingData){   
        pthread_mutex_lock(threadData->queueMutex); // Lock then check.

        // reading validate conditions
        if (threadData->readData->size() >= queueFull){
            std::cout << "queue full, waiting..." << std::endl;
            pthread_cond_broadcast(threadData->condQueueWrite);
            pthread_cond_wait(threadData->condQueueRead, threadData->queueMutex);

            if (inputFile.eof()){
                std::cout << "end triggered" << std::endl;
                readingData = false;
                pthread_exit(NULL);
                return NULL;
            }

        }
        if(readingData){
            // reading process
            std::getline(inputFile, line);
            threadData->readData->push(line);
            usleep(10000);
            std::cout << "reading: " << line << std::endl;

        }
        pthread_mutex_unlock(threadData->queueMutex);
        pthread_cond_signal(threadData->condQueueWrite);

    }
    std::cout << "read thread lost" << std::endl;
    return NULL;
};

void* write_call(void* args){

    // wake up the readers if the queue has 0 items to write

    ThreadData* threadData = static_cast<ThreadData*>(args);
    std::ofstream outputFile(threadData->destinationFile);

    sleep(1);
    while(true){
        pthread_mutex_lock(threadData->queueMutex); // Lock then check


        while (threadData->readData->empty()){
            if(!std::filesystem::exists(threadData->sourceFile) && threadData->readData->empty()){
                pthread_mutex_unlock(threadData->queueMutex);
                pthread_exit(NULL);
                return NULL;
            }

            std::cout << "Queue is empty, waiting..." << std::endl;
            pthread_cond_broadcast(threadData->condQueueRead);
            pthread_cond_wait(threadData->condQueueWrite, threadData->queueMutex);
        }

        outputFile << threadData->readData->front() << std::endl;
        std::cout << "writing: " << threadData->readData->front() << std::endl;
        threadData->readData->pop();
        usleep(10000);
        pthread_mutex_unlock(threadData->queueMutex);
        pthread_cond_signal(threadData->condQueueRead);
    }
    std::cout << "write thread lost" << std::endl;
    //write_from_queue(threadData->destinationFile);
    return NULL;
};
