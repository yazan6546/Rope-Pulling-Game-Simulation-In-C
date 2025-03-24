//
// Created by - on 3/24/2025.
//

#include "file.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/stat.h>

// // Returns full path to executable (Linux-specific)
// char* get_executable_path() {
//     char buffer[PATH_MAX];
//     ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
//     if (len == -1) return NULL;
//     buffer[len] = '\0';
//     return strdup(buffer);
// }

// Finds config.txt relative to executable's directory
char* find_config_file(char *argv) {
    char* exe_path = argv;
    if (!exe_path) return NULL;

    // Get executable's directory
    char exe_dir[PATH_MAX];
    strncpy(exe_dir, exe_path, PATH_MAX);
    dirname(exe_dir); // Strips executable name from path

    // Possible relative paths to check (adjust as needed)
    const char* candidates[] = {
        "config.txt",            // Same dir as executable
        "../config.txt",         // One level up
        "../../config.txt",      // Two levels up
        "cfg/config.txt"         // Subdirectory
    };

    // Check each candidate
    for (size_t i = 0; i < sizeof(candidates)/sizeof(candidates[0]); i++) {
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", exe_dir, candidates[i]);

        struct stat st;
        if (stat(full_path, &st) == 0 && S_ISREG(st.st_mode)) {
            // free(exe_path);
            return strdup(full_path);
        }
    }

    // free(exe_path);
    return NULL;
}

void handling_file(int argc, char* argv, char **config_path) {

    // Check command-line argument first
    if (argc > 1) {
        *config_path = argv;
    } else {
        *config_path = find_config_file(argv);
    }

    if (!(*config_path)) {
        perror("Config file");
        exit(1);
    }

    // Use config_path (e.g., fopen(config_path, "r"))
    printf("Using config: %s\n", *config_path);
}

char* binary_dir(const char* config_dir) {
    if (!config_dir) return NULL;

    // Make a copy as dirname modifies its argument
    char* dir_copy = strdup(config_dir);
    if (!dir_copy) return NULL;

    // Get the directory containing the config file
    dirname(dir_copy);

    // Allocate memory for the bin directory path
    char* bin_path = malloc(PATH_MAX);
    if (!bin_path) {
        free(dir_copy);
        return NULL;
    }

    // Construct the path to the bin directory
    snprintf(bin_path, PATH_MAX, "%s/bin", dir_copy);

    // Clean up
    free(dir_copy);
    return bin_path;
}