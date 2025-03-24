#include "common.h"
#include "config.h"
#include "random.h"
#include "player_utils.h"
#include "file.h"
#include "player.h"

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

#define PATH_MAX 4096

Config config;

void fork_players(Team team, int num_players, char *bin_path, int pipe_fds[]);

int main(int argc, char *argv[]) {

    char *config_path = NULL;
    handling_file(argc, argv[0], &config_path);
    char *bin_path = binary_dir(config_path);

    printf("Config path: %s\n", config_path);
    load_config(config_path, &config);
    print_config(&config);

    int pipe_fds_team_A[config.NUM_PLAYERS/2];
    int pipe_fds_team_B[config.NUM_PLAYERS/2];

    fork_players(TEAM_A, config.NUM_PLAYERS/2, bin_path, pipe_fds_team_A);
    fork_players(TEAM_B, config.NUM_PLAYERS/2, bin_path, pipe_fds_team_B);

    while (1) {
        float total_A = 0.0, total_B = 0.0;

        for (int i = 0; i < config.NUM_PLAYERS/2; i++) {
            float energy;
            ssize_t bytes = read(pipe_fds_team_A[i], &energy, sizeof(float));
            if (bytes == sizeof(float)) {
                printf("Team A - Player %d energy: %.2f\n", i, energy);
                total_A += energy;
            }
        }

        for (int i = 0; i < config.NUM_PLAYERS/2; i++) {
            float energy;
            ssize_t bytes = read(pipe_fds_team_B[i], &energy, sizeof(float));
            if (bytes == sizeof(float)) {
                printf("Team B - Player %d energy: %.2f\n", i, energy);
                total_B += energy;
            }
        }

        int score = total_A - total_B;
        printf("\nTotal Energy A: %.2f | Total Energy B: %.2f | Score: %d\n\n", total_A, total_B, score);

        if (score >= config.MAX_SCORE) {
            printf("🏆 Team A wins!\n");
            break;
        } else if (score <= -config.MAX_SCORE) {
            printf("🏆 Team B wins!\n");
            break;
        }

        sleep(1);
    }

    free(bin_path);
    return 0;
}


void fork_players(Team team, int num_players, char *binary_path, int pipe_fds[]) {
    Player players[num_players];

    for (int i = 0; i < num_players; i++) {
        int pipefd[2];
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
            close(pipefd[0]); // child closes read end

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
            close(pipefd[1]); // parent closes write end
            pipe_fds[i] = pipefd[0]; // store read end
        }
    }
}
