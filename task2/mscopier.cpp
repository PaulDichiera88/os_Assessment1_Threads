#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <string>
#include <filesystem>
#include <vector>
#include <fstream>
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

    std::cout << "** Performing Checks **" << std::endl;

    if (argc != 4){
        std::cout << "required input: <number of files to copy, integer> <source folder> <destination folder> " << std::endl;
        return EXIT_FAILURE;
    }

    int threadNum = std::stoi(argv[1]);     //number of threads to use in operations
    std::string copySource = argv[2];       //save source file 
    std::string copyDestination = argv[3];  // save destination file
    std::queue<std::string> string_queue;   // container for source data
    pthread_mutex_t queueMutex;             // Mutex lock
    pthread_cond_t condQueueRead;           // reader condition
    pthread_cond_t condQueueWrite;          // writer condition

    // source file check and open
    std::ifstream inputFile;
    if(std::filesystem::exists(copySource)){ 
        inputFile.open(copySource);
        std::cout << "File location: Success... " << copySource << std::endl;
        if(!inputFile.is_open()){
            std::cerr << "Error: Could not open source file " << copySource << std::endl;
            return EXIT_FAILURE;
        }
    } else {
        std::cout << "File location: Failed... " << "Source file is incorrect or does not exist" << std::endl;
        return EXIT_FAILURE;
    }

    // destination file check, open and creation if required
    std::ofstream outputFile;
    if(std::filesystem::exists(copyDestination)){
        outputFile.open(copyDestination);
        if(!outputFile.is_open()){
            std::cerr << "Error: Could not open destination file " << copyDestination << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "File location: Success... " << copyDestination << std::endl;
    } else {
        std::cout << "File location: Failed... " << "Destination file name is incorrect or does not exist. Creating " << copyDestination << std::endl;
        outputFile.open(copyDestination);
        if(!outputFile.is_open()){
            std::cerr << "Error: Could not open destination file " << copyDestination << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "** A destination file has been created **" << std::endl;
    }

    std::cout << "** Starting Operation **" << std::endl;

    // initialize mutex and condition variables
    pthread_cond_init(&condQueueRead, NULL);
    pthread_cond_init(&condQueueWrite, NULL);
    pthread_mutex_init(&queueMutex, NULL);

    // allocate and populate ThreadData struct
    ThreadData* threadData = new ThreadData;

    threadData->sourceFile = copySource;
    threadData->destinationFile = copyDestination;
    threadData->readData = &string_queue;
    threadData->queueMutex = &queueMutex;
    threadData->condQueueRead = &condQueueRead;
    threadData->condQueueWrite = &condQueueWrite;
    threadData->inputFile = &inputFile;
    threadData->outputFile = &outputFile;

    // initialisation of thread conainers
    std::vector<pthread_t> readThreads;
    std::vector<pthread_t> writeThreads;

    // initialisation of read threads, call read function
    for (int i = 0; i < threadNum; i++){
        pthread_t thread;

        if(pthread_create(&thread, NULL, read_call, threadData) != 0){
            std::cout << "read thread creation failed" << std::endl; 
        } else {
            readThreads.push_back(thread);
        }
    }

    // initialisation of write threads, call write function
    for (int i = 0; i < threadNum; i++){
        pthread_t thread;

        if(pthread_create(&thread, NULL, write_call, threadData) != 0){
            std::cout << "write thread creation failed" << std::endl;
        } else {
            writeThreads.push_back(thread);
        }
    }

    // join threads
    for(pthread_t& thread : readThreads){
        pthread_join(thread, NULL);
    }
    for(pthread_t& thread : writeThreads){
        pthread_join(thread, NULL);
    }

    // declete allocated memory
    pthread_mutex_destroy(threadData->queueMutex);
    pthread_cond_destroy(threadData->condQueueRead);
    pthread_cond_destroy(threadData->condQueueWrite);

    // closing files and freeing memory
    threadData->inputFile->close();
    threadData->outputFile->close();
    delete threadData;

    std::cout << "** Operation Complete **" << std::endl;
    std::cout << "** Have a lovey day :) **" << std::endl;

    return EXIT_SUCCESS;
}

// reads from input file and manages threads
void* read_call(void* args){
    ThreadData* threadData = static_cast<ThreadData*>(args);
    int queueFull = 20;
    std::string line;
    bool readingData = true;

    while(readingData){   
        pthread_mutex_lock(threadData->queueMutex);

        // reading validate conditions
        while (threadData->readData->size() >= queueFull){
            pthread_cond_broadcast(threadData->condQueueWrite);
            pthread_cond_wait(threadData->condQueueRead, threadData->queueMutex);
        }
        if (threadData->inputFile->eof()){
            pthread_cond_broadcast(threadData->condQueueWrite);
            threadData->readComplete = true;
            readingData = false;

        } else if (readingData){
            std::getline(*threadData->inputFile, line);
            threadData->readData->push(line);

        }
        pthread_mutex_unlock(threadData->queueMutex);
        pthread_cond_signal(threadData->condQueueWrite);

    }
    pthread_exit(NULL);
    return NULL;
};

// writes from container and manages threads
void* write_call(void* args){
    ThreadData* threadData = static_cast<ThreadData*>(args);
    bool writingData = true;

    while(writingData){
        pthread_mutex_lock(threadData->queueMutex); // Lock then check

        while (threadData->readData->empty() && !threadData->readComplete){
            pthread_cond_broadcast(threadData->condQueueRead);
            pthread_cond_wait(threadData->condQueueWrite, threadData->queueMutex);
        }

        if(threadData->readComplete && threadData->readData->empty()){
                writingData = false;
        } else {
            *threadData->outputFile << threadData->readData->front() << std::endl;
            threadData->readData->pop();
        }

        pthread_mutex_unlock(threadData->queueMutex);
        pthread_cond_signal(threadData->condQueueRead);
    }
    pthread_exit(NULL);
    return NULL;
};
