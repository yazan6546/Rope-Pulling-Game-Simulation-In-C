#include "common.h"
#include "config.h"
#include "random.h"
#include <signal.h>
#include "referee_orders.h"
#include "player_utils.h"
#include "file.h"


#define PATH_MAX 4096
Config config;

void fork_players(Team team, int num_players, char *binary_path, pid_t* team_pids);
int main(int argc, char *argv[]) {

    char *config_path = NULL;
    handling_file(argc, argv[0], &config_path);
    char *bin_path = binary_dir(config_path);


    // Allocate memory for player PIDs
    pid_t team_a_pids[sizeof(pid_t) * config.NUM_PLAYERS/2];
    pid_t team_b_pids[sizeof(pid_t) * config.NUM_PLAYERS/2];

    load_config(config_path, &config);

    fork_players(TEAM_A, config.NUM_PLAYERS/2, bin_path, team_a_pids);
    fork_players(TEAM_B, config.NUM_PLAYERS/2, bin_path, team_b_pids);

    wait(NULL);


    // Send get ready signal to all players
    for (int i = 0; i < config.NUM_PLAYERS/2; i++) {
        kill(team_a_pids[i], SIGUSR1);
        kill(team_b_pids[i], SIGUSR1);
    }

    // the referee aligns the players, why send a signal to the players to align themselves?
    align(team_a_pids, config.NUM_PLAYERS/2);
    align(team_b_pids, config.NUM_PLAYERS/2);

    sleep(2); // Wait for players to get ready

    // Send start signal to all players
    for (int i = 0; i < config.NUM_PLAYERS/2; i++) {
        kill(team_a_pids[i], SIGUSR2);
        kill(team_b_pids[i], SIGUSR2);
    }

    while (1) {}

    return 0;

    free(bin_path);
}

void fork_players(Team team, int num_players, char *binary_path, pid_t* team_pids) {

    Player players[num_players];
    int pipefd[2];

    for (int i = 0; i < num_players; i++) {
        if (pipe(pipefd) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        const pid_t pid = fork();
        init_random(getpid());

        if (pid == -1) {
            perror("fork");
        }

        else if (pid == 0) {
            close(pipefd[0]); // Close read end in child

            char buffer[100];
            generate_random_player(&players[i], &config, team, i);
            serialize_player(&players[i], buffer);

            char player_path[PATH_MAX];
            snprintf(player_path, PATH_MAX, "%s/player", binary_path);

            char write_fd_str[10];
            sprintf(write_fd_str, "%d", pipefd[1]);

            if (execl(player_path, "player", buffer, write_fd_str, NULL)) {
                perror("execl");
                exit(1);
            }
        }

        else {
            close(pipefd[1]); // Close write end in parent
            // Store PID in appropriate array
            team_pids[i] = pid;
            printf("Parent process spawned player with PID: %d\n", pid);
            fflush(stdout);


            float energy;
            ssize_t bytes_read = read(pipefd[0], &energy, sizeof(float));
            if (bytes_read == sizeof(float)) {
                printf("Received energy from player %d: %.2f\n", i, energy);
            }
            else {
                printf("Failed to read energy from player %d\n", i);
            }

            close(pipefd[0]);

        }
    }
}


