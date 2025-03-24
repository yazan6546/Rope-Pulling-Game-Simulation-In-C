//
// Created by - on 3/24/2025.
//

#ifndef FILE_H
#define FILE_H

char* find_config_file(char *argv);
char* get_executable_path();
void handling_file(int argc, char* argv, char **config_path);
char* binary_dir(const char* config_dir);

#endif //FILE_H
