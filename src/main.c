#include "common.h"
#include <signal.h>
#include "game.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "config.h"

// Global pointer to shared game state
Game *shared_game;
int shm_fd; // Store fd globally for cleanup

pid_t pid_graphics;
pid_t pid_referee;

void handle_alarm(int signum);
void handle_sigint(int signum); // Renamed from handle_sigkill to match actual signal
void cleanup_resources(void);   // New function for atexit
pid_t start_graphics_process(Config *config, char *fd_str);
pid_t start_referee_process(Config *config, char *fd_str, char *gui_pid_str);

void handle_alarm(int signum) {

    // Check if referee requested a reset
    if (shared_game->reset_round_time_flag) {
        shared_game->round_time = 0;
    } else {
        shared_game->round_time++;
        shared_game->elapsed_time++;
    }

    alarm(1);
}

void handle_sigint(int signum) {
    exit(0); // Let atexit handle cleanup
}

// Function for cleaning up resources, registered with atexit()
void cleanup_resources(void) {
    printf("Cleaning up resources...\n");
    fflush(stdout);

    if (shared_game != NULL && shared_game != MAP_FAILED) {
        if (munmap(shared_game, sizeof(Game)) == -1) {
            perror("munmap failed");
        }
    }

    shm_unlink("/game_shared_mem");

    if (shm_fd > 0) {
        close(shm_fd);
    }

    kill(pid_graphics, SIGKILL);
    kill(pid_referee, SIGKILL);

    printf("Cleanup complete\n");
    fflush(stdout);
}

int main(int argc, char *argv[]) {

    printf("********** The Rope Pulling Game **********\n\n");
    fflush(stdout);

    Config config;

    if (load_config("../config.txt", &config) == -1) {
        return 1;
    }

    // Register cleanup function with atexit
    atexit(cleanup_resources);

    // shared_game = malloc(sizeof(Game)); // Remove this line - using mmap instead

    // Create shared game state using mmap
    shm_fd = shm_open("/game_shared_mem", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, sizeof(Game)) == -1) {
        perror("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    shared_game = mmap(NULL, sizeof(Game), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_game == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    fcntl(shm_fd, F_SETFD, fcntl(shm_fd, F_GETFD) & ~FD_CLOEXEC);

    // Pass fd as string
    char fd_str[16];
    sprintf(fd_str, "%d", shm_fd);

    // Initialize game state
    init_game(shared_game);
    //
    pid_graphics = start_graphics_process(&config, fd_str);
    // Pass the graphics pid additionally to referee exec call.
    char gui_pid_str[16];
    sprintf(gui_pid_str, "%d", pid_graphics);
    pid_referee = start_referee_process(&config, fd_str, gui_pid_str);

    // Setup signal handler for time management
    signal(SIGALRM, handle_alarm);
    signal(SIGINT, handle_sigint); // Renamed to match actual signal
    alarm(1);  // Start the timer

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

        return 0; // cleanup_resources will be called automatically via atexit
    }
}


pid_t start_graphics_process(Config *config, char *fd_str) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        char buffer[50];
        serialize_config(config, buffer);
        if (execlp("./rope_game_graphics", "rope_pulling_game_main", buffer, fd_str, NULL)) {
            perror("execl graphics");
            exit(EXIT_FAILURE);
        }
    }
    return pid;
}

pid_t start_referee_process(Config *config, char *fd_str, char *gui_pid_str) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Now pass two arguments: shared memory fd and GUI pid.
        char buffer[50];
        serialize_config(config, buffer);
        if (execl("./referee", "./referee", buffer, fd_str, gui_pid_str, NULL)) {
            perror("execl referee");
            exit(EXIT_FAILURE);
        }
    }
    return pid;
}