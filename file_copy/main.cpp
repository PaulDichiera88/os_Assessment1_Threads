#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <string>
#include <filesystem>
#include <vector>
#include <dirent.h>

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



int main(int argc, char* argv[]){

    if (argc != 4){
        std::cout << "required input: <number of files to copy, integer> <source folder> <destination folder> " << std::endl;
    }

    int copyNum = std::stoi(argv[1]); //take number of file to be copied
    std::string copySource = argv[2]; //save source file 
    std::string copyDestination = argv[3]; // save destination file

    std::vector<std::string> filenames = {


    }

    // Check source directory exists and contents

    // Ensure destination directory exists


    return EXIT_SUCCESS;
}