//
// Created by - on 3/25/2025.
//

#include "common.h"

pid_t start_graphics_process();
pid_t start_referee_process();

int main(int argc, char *argv[]) {

    pid_t pid = start_graphics_process();
    start_referee_process();
    wait(NULL);

    int status;
    waitpid(pid, &status, 0);


    if (WIFEXITED(status)) {
        printf("Child process exited with code: %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("Child process terminated by signal: %d\n", WTERMSIG(status));
    }

}

pid_t start_graphics_process() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        if (execlp("./rope_game_graphics", "rope_pulling_game_main", NULL)) {
            perror("execl graphics");
            exit(EXIT_FAILURE);
        }

    }

    return pid;
}

pid_t start_referee_process() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        if (execl("./referee", "./referee", NULL)) {
            perror("execl referee");
            exit(EXIT_FAILURE);
        }
    }

    return pid;
}