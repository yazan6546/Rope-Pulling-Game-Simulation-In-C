#include "common.h"
#include <sys/mman.h>
#include <signal.h>
#include "game.h"

// Global pointer to shared game state
Game *shared_game;

void handle_alarm(int signum);
pid_t start_graphics_process(char *shared_addr);
pid_t start_referee_process(char *shared_addr);

void handle_alarm(int signum) {
    shared_game->elapsed_time++;

    // Check if referee requested a reset
    if (shared_game->reset_round_time_flag) {
        shared_game->round_time = 0;
        shared_game->reset_round_time_flag = 0; // Reset the flag
    } else {
        shared_game->round_time++;
    }

    alarm(1);
}

int main(int argc, char *argv[]) {
    // Create shared game state using mmap
    shared_game = mmap(NULL, sizeof(Game),
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (shared_game == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    // Initialize game state
    init_game(shared_game);

    // Setup signal handler for time management
    signal(SIGALRM, handle_alarm);
    alarm(1);  // Start the timer

    // Convert shared memory address to string for child processes
    char shared_addr[32];
    sprintf(shared_addr, "%p", (void*)shared_game);

    pid_t pid_graphics = start_graphics_process(shared_addr);
    pid_t pid_referee = start_referee_process(shared_addr);

    int status_referee, status_graphics;
    waitpid(pid_referee, &status_referee, 0);

    if (WIFEXITED(status_referee)) {
        printf("Referee Child process exited with code: %d\n", WEXITSTATUS(status_referee));
    } else if (WIFSIGNALED(status_referee)) {
        printf("Referee Child process terminated by signal: %d\n", WTERMSIG(status_referee));
    }

    waitpid(pid_graphics, &status_graphics, 0);

    if (WIFEXITED(status_graphics)) {
        printf("Graphics Child process exited with code: %d\n", WEXITSTATUS(status_graphics));
    } else if (WIFSIGNALED(status_graphics)) {
        printf("Graphics Child process terminated by signal: %d\n", WTERMSIG(status_graphics));
    }

    // Clean up shared memory
    if (munmap(shared_game, sizeof(Game)) == -1) {
        perror("munmap failed");
        exit(EXIT_FAILURE);
    }


    return 0;
}

pid_t start_graphics_process(char *shared_addr) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        if (execlp("./rope_game_graphics", "rope_pulling_game_main", shared_addr, NULL)) {
            perror("execl graphics");
            exit(EXIT_FAILURE);
        }
    }
    return pid;
}

pid_t start_referee_process(char *shared_addr) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        if (execl("./referee", "./referee", shared_addr, NULL)) {
            perror("execl referee");
            exit(EXIT_FAILURE);
        }
    }
    return pid;
}