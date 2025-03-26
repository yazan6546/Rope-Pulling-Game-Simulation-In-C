#include "common.h"
#include "config.h"
#include "random.h"
#include <signal.h>
#include <wchar.h>

#include "referee_orders.h"
#include "player_utils.h"
#include "file.h"
#include "player.h"


#define PATH_MAX 4096
Config config;

void fork_players(Player *players, int num_players, Team team, char *binary_path, int pipe_fds[]);
void generate_and_align(Player *players, int num_players, Team team);

int main(int argc, char *argv[]) {

    char *config_path = NULL;
    handling_file(argc, argv[0], &config_path);
    char *bin_path = binary_dir(config_path);


    load_config(config_path, &config);

    Player players_teamA[config.NUM_PLAYERS/2];
    Player players_teamB[config.NUM_PLAYERS/2];


    int pipe_fds_team_A[config.NUM_PLAYERS/2];
    int pipe_fds_team_B[config.NUM_PLAYERS/2];


    generate_and_align(players_teamA, config.NUM_PLAYERS/2, TEAM_A);
    generate_and_align(players_teamB, config.NUM_PLAYERS/2, TEAM_B);

    fork_players(players_teamA, config.NUM_PLAYERS/2, TEAM_A, bin_path, pipe_fds_team_A);
    fork_players(players_teamB, config.NUM_PLAYERS/2, TEAM_B, bin_path, pipe_fds_team_B);


    sleep(2); // Wait for players to get ready

    printf("\n\n");

    // Send get ready signal to all players
    for (int i = 0; i < config.NUM_PLAYERS/2; i++) {
        kill(players_teamA[i].pid, SIGUSR1);
        kill(players_teamB[i].pid, SIGUSR1);
    }

    printf("\n\n");

    sleep(3); // Wait for players to get ready
    //
    // Send start signal to all players
    for (int i = 0; i < config.NUM_PLAYERS/2; i++) {
        kill(players_teamA[i].pid, SIGUSR2);
        kill(players_teamB[i].pid, SIGUSR2);
    }
    printf("\n\n");

    sleep(3);



    while (1) {
        float total_A = 0.0f, total_B = 0.0f;

        for (int i = 0; i < config.NUM_PLAYERS/2; i++) {
            float effort;
            ssize_t bytes = read(pipe_fds_team_A[i], &effort, sizeof(float));


            if (bytes == sizeof(float) || bytes == 0) {
                printf("Team A - Player %d effort: %.2f\n", i, effort);
                total_A += effort;
            }
        }

        for (int i = 0; i < config.NUM_PLAYERS/2; i++) {
            float effort;
            ssize_t bytes = read(pipe_fds_team_B[i], &effort, sizeof(float));
            if (bytes == sizeof(float) || bytes == 0) {
                printf("Team B - Player %d effort: %.2f\n", i, effort);
                total_B += effort;
            }
        }

        float score = total_A - total_B;
        printf("\nTotal Effort A: %.2f | Total Effort B: %.2f | Score: %.2f\n\n", total_A, total_B, score);

        if (score >= config.MAX_SCORE) {
            printf("🏆 Team A wins!\n");
            break;
        }
        if (score <= -config.MAX_SCORE) {
            printf("🏆 Team B wins!\n");
            break;
        }

        sleep(1);
    }

    free(bin_path);
    return 0;
}

void fork_players(Player *players, int num_players, Team team, char *binary_path, int pipe_fds[]) {

    for (int i = 0; i < num_players; i++) {

        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        const pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
        }

        else if (pid == 0) {

            close(pipefd[0]); // Close read end in child

            char buffer[100];
            serialize_player(&players[i], buffer);

            char player_path[PATH_MAX];
            snprintf(player_path, PATH_MAX, "%s/player", binary_path);

            char write_fd_str[10];
            sprintf(write_fd_str, "%d", pipefd[1]);

            // printf("%s\n", player_path);

            if (execl(player_path, "player", buffer, write_fd_str, NULL)) {
                perror("execl");
                exit(1);
            }
        }

        else {
            players[i].pid = pid;
            close(pipefd[1]); // parent closes write end
            pipe_fds[i] = pipefd[0]; // store read end
        }
    }
}

void generate_and_align(Player *players, int num_players, Team team) {

    for (int i = 0; i<num_players; i++) {
        generate_random_player(&players[i], &config, team, i);
    }

    align(players, num_players);


}


