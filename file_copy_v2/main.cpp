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

std::vector<std::string> get_filenames(const std::string& copySource);
//create a thread function that copies a file
//the file is passed to the thread, it is read and copied
//then saved to a new destination
//the file source and destination are input when the program is run.

//the read and copy should happen in the thread
//the source file, source folkder and destination should be validated before the thread is run

//int argc = number of arguments will be 4 overall
//argv[0] = ./mmcopier
//argv[1] = 3 (for example)
//argv[2] = source.dir
//argv[3] = destination.dir

struct ThreadDirectories {
    std::string source_file;
    std::string destination_file;
};

void* thread_copy_call(void* arg);
void copy_file_call(const std::string& source, const std::string& destination);


int main(int argc, char* argv[]){

    if (argc != 4){
        std::cout << "required input: <number of files to copy, integer> <source folder> <destination folder> " << std::endl;
    }

    int copyNum = std::stoi(argv[1]); //take number of file to be copied
    std::string copySource = argv[2]; //save source file 
    std::string copyDestination = argv[3]; // save destination file

    std::vector<std::string> filenames = get_filenames(copySource);

    if(copyNum > filenames.size()){
        copyNum = filenames.size();
    }

    std::vector<pthread_t> threads;

    for( int i = 0; i < copyNum; i++){
        //call threads push thread to thread vector.
        pthread_t thread;

        //prepare struct
        ThreadDirectories* T_dir = new ThreadDirectories;
        T_dir->source_file = copySource + "/" + filenames[i];
        T_dir->destination_file = copyDestination + "/" + filenames[i];

        if(pthread_create(&thread, NULL, thread_copy_call, T_dir) != 0){
            delete T_dir;
        }else{
            threads.push_back(thread);
        }
    }

    // Join threads
    for (pthread_t& thread : threads) {
        pthread_join(thread, NULL);
    }


    // Check source directory exists and contents

    // Ensure destination directory exists


    return EXIT_SUCCESS;
}

std::vector<std::string> get_filenames(const std::string& copySource){
    std::vector<std::string> filenames;

    DIR* dir = opendir(copySource.c_str());

    struct dirent* entry;
    while(( entry = readdir(dir)) != nullptr){
        if(!(std::strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)){
            filenames.push_back(entry->d_name);
        }    
    }
    closedir(dir);
    std::sort(filenames.begin(), filenames.end());

    return filenames;
}

void* thread_copy_call(void* args){
    ThreadDirectories* Thread_Dirs = static_cast<ThreadDirectories*>(args);

    copy_file_call(Thread_Dirs->source_file, Thread_Dirs->destination_file);

    delete Thread_Dirs;
    pthread_exit(NULL);
    return NULL;

};

void copy_file_call(const std::string& source, const std::string& destination){
    std::ifstream src(source, std::ios::binary);
    std::ofstream dst(destination, std::ios::binary);

    if(!src.is_open()){
        std::cerr << "ERROR: Could not open source file: " << source << std::endl;
        return; 
    }

    dst << src.rdbuf();
    std::cout << "File successfully copied from " << source << " to " << destination << std::endl;
};