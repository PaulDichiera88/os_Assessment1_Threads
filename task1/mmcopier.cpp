#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>  // For stat and mkdir
#include <vector>
#include <dirent.h>    // For directory operations
#include <cstring>
#include <algorithm>
#include <fstream>
#include <queue>
#include <mutex>

// Function declarations
std::vector<std::string> get_filenames(const std::string& copySource);
void* thread_copy_call(void* arg);
void copy_file_call(const std::string& source, const std::string& destination);
bool directory_exists(const std::string& path);
bool create_directory(const std::string& path);

// Structure for destination and source file paths
struct ThreadDirectories {
    std::string source_file;
    std::string destination_file;
};

int main(int argc, char* argv[]) {

    std::cout << "** Performing Checks **" << std::endl;

    // Input validation
    if (argc != 4) {
        std::cerr << "Required input: <number of files to copy, integer> <source folder> <destination folder>" << std::endl;
        return EXIT_FAILURE;
    }

    // Process input arguments
    int copyNum = std::stoi(argv[1]); // Number of files to be copied
    std::string copySource = argv[2]; // Source directory
    std::string copyDestination = argv[3]; // Destination directory

    // Validate source directory
    if (!directory_exists(copySource)) {
        std::cerr << "Error: Source directory does not exist or is not a directory." << std::endl;
        return EXIT_FAILURE;
    }

    // Ensure destination directory exists, create if it doesn't
    if (!directory_exists(copyDestination)) {
        std::cout << "Directory location: Failed... Destination directory does not exist. Creating " << copyDestination << std::endl;
        if (!create_directory(copyDestination)) {
            std::cerr << "Error: Could not create destination directory." << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "** A destination directory has been created **" << std::endl;
    }

    std::cout << "** Starting Operation **" << std::endl;

    // Store filenames from the directory
    std::vector<std::string> filenames = get_filenames(copySource);

    if (copyNum > filenames.size()) {
        copyNum = filenames.size();
    }

    // Create a vector to store threads
    std::vector<pthread_t> threads;

    // Create threads and call the copy function
    for (int i = 0; i < copyNum; i++) {
        pthread_t thread;

        // Allocate memory and populate struct
        ThreadDirectories* threadDir = new ThreadDirectories;
        std::string filename = filenames[i];
        threadDir->source_file = copySource + "/" + filename;
        threadDir->destination_file = copyDestination + "/" + filename;

        if (pthread_create(&thread, NULL, thread_copy_call, threadDir) != 0) {
            delete threadDir;
        } else {
            threads.push_back(thread);
        }
    }

    // Join threads
    for (pthread_t& thread : threads) {
        pthread_join(thread, NULL);
    }

    std::cout << "** Operation Complete **" << std::endl;
    std::cout << "** Have a lovely day :) **" << std::endl;

    return EXIT_SUCCESS;
}

// Retrieve and sort filenames from source directory
std::vector<std::string> get_filenames(const std::string& copySource) {
    std::vector<std::string> filenames;

    DIR* dir = opendir(copySource.c_str());
    if (dir == nullptr) {
        std::cerr << "Error: Could not open source directory." << std::endl;
        return filenames;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (!(std::strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)) {
            filenames.push_back(entry->d_name);
        }
    }
    closedir(dir);

    // Sort to ensure order
    std::sort(filenames.begin(), filenames.end(), std::locale("en_US.UTF-8"));

    return filenames;
}

void* thread_copy_call(void* args) {
    ThreadDirectories* Thread_Dirs = static_cast<ThreadDirectories*>(args);

    // File copy
    copy_file_call(Thread_Dirs->source_file, Thread_Dirs->destination_file);

    // Free memory 
    delete Thread_Dirs;

    // Exit thread
    pthread_exit(NULL);
    return NULL;
}

void copy_file_call(const std::string& source, const std::string& destination) {
    std::ifstream src(source, std::ios::binary);
    std::ofstream dst(destination, std::ios::binary);

    if (!src.is_open()) {
        std::cerr << "ERROR: Could not open source file: " << source << std::endl;
        return;
    }

    if (!dst.is_open()) {
        std::cerr << "Error: Could not open destination file: " << destination << std::endl;
        return;
    }

    dst << src.rdbuf();
}

// Check if a directory exists
bool directory_exists(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false;  // Cannot access path
    } else if (info.st_mode & S_IFDIR) {
        return true;   // Path is a directory
    } else {
        return false;  // Path is not a directory
    }
}

// Create a directory (recursive)
bool create_directory(const std::string& path) {
    mode_t mode = 0755;  // Default permissions
    return mkdir(path.c_str(), mode) == 0 || errno == EEXIST;
}
