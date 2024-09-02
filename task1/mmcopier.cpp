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

// function declarations
std::vector<std::string> get_filenames(const std::string& copySource);
void* thread_copy_call(void* arg);
void copy_file_call(const std::string& source, const std::string& destination);

// structure for destination and source file paths
struct ThreadDirectories {
    std::string source_file;
    std::string destination_file;
};

int main(int argc, char* argv[]){

    std::cout << "** Performing Checks **" << std::endl;

    // input validation
    if (argc != 4){
        std::cout << "required input: <number of files to copy, integer> <source folder> <destination folder> " << std::endl;
    }

    // process input arguments
    int copyNum = std::stoi(argv[1]); //take number of file to be copied
    std::string copySource = argv[2]; //save source file 
    std::string copyDestination = argv[3]; // save destination file
    const std::filesystem::path fileDir{copySource};

    // validate source directory
    if (!std::filesystem::exists(copySource) || !std::filesystem::is_directory(copySource)){
        std::cerr << "Error: Source directory does now exist or is not a directory." << std::endl;
        return EXIT_SUCCESS;
    }

    if(!std::filesystem::exists(copyDestination)){
        std::cout << "Directory location: Failed... " << "Destination directory name is incorrect or does not exist. Creating " << copyDestination << std::endl;
        if (!std::filesystem::create_directories(copyDestination)){
            std::cerr << "Error: Could not create destination directory." << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "** A destination directory has been created **" << std::endl;
    }

    std::cout << "** Starting Operation **" << std::endl;

    // store filenames from directory
    std::vector<std::string> filenames = get_filenames(copySource);

    if(copyNum > static_cast<int>(filenames.size())){
        copyNum = filenames.size();
    }

    // create vector to store threads
    std::vector<pthread_t> threads;

    // create threads and call copy function
    for( int i = 0; i < copyNum; i++){
        pthread_t thread;

        // allocated memory and populate struct
        ThreadDirectories* threadDir = new ThreadDirectories;
        std::string filename = "source" + std::to_string(i + 1) + ".txt";
        threadDir->source_file = copySource + "/" + filename;
        threadDir->destination_file = copyDestination + "/" + filename;

        if(pthread_create(&thread, NULL, thread_copy_call, threadDir) != 0){
            delete threadDir;
        }else{
            threads.push_back(thread);
        }
    }

    // Join threads
    for (pthread_t& thread : threads) {
        pthread_join(thread, NULL);
    }

    std::cout << "** Operation Complete **" << std::endl;
    std::cout << "** Have a lovey day :) **" << std::endl;

    return EXIT_SUCCESS;
}

    // retrive and sort filenames from source directory
std::vector<std::string> get_filenames(const std::string& copySource){
    std::vector<std::string> filenames;

    DIR* dir = opendir(copySource.c_str());
    if(dir == nullptr){
        std::cerr << "Error: Could not open source directory." << std::endl;
        return filenames;
    }

    struct dirent* entry;
    while(( entry = readdir(dir)) != nullptr){
        if(!(std::strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)){
            filenames.push_back(entry->d_name);
        }    
    }
    closedir(dir);

    //sort to ensure order
    std::sort(filenames.begin(), filenames.end(), std::locale("en_US.UTF-8"));

    return filenames;
}

void* thread_copy_call(void* args){
    ThreadDirectories* Thread_Dirs = static_cast<ThreadDirectories*>(args);

    // file copy
    copy_file_call(Thread_Dirs->source_file, Thread_Dirs->destination_file);

    // free memory 
    delete Thread_Dirs;

    // exit thread
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

    if(!dst.is_open()){
        std::cerr << "Error: Could not open source file: " << destination << std::endl;
        return;
    }

    dst << src.rdbuf();

};