#include "common.h"
#include "config.h"
#include "random.h"
#include <signal.h>
#include "referee_orders.h"
#include "player_utils.h"
#include "file.h"


#define PATH_MAX 4096
Config config;

void fork_players(Player *players, int num_players, Team team, char *binary_path);
int main(int argc, char *argv[]) {

    printf("%s\n", argv[0]);

    Player players_teamA[config.NUM_PLAYERS/2];
    Player players_teamB[config.NUM_PLAYERS/2];

    char *config_path = NULL;
    handling_file(argc, argv[0], &config_path);
    char *bin_path = binary_dir(config_path);


    load_config(config_path, &config);

    fork_players(players_teamA, config.NUM_PLAYERS/2, TEAM_A, bin_path);
    fork_players(players_teamB, config.NUM_PLAYERS/2, TEAM_B, bin_path);

    wait(NULL);

    // Send get ready signal to all players
    for (int i = 0; i < config.NUM_PLAYERS/2; i++) {
        kill(players_teamA[i].pid, SIGUSR1);
        kill(players_teamB[i].pid, SIGUSR1);
    }

    align(players_teamA, config.NUM_PLAYERS/2);
    align(players_teamB, config.NUM_PLAYERS/2);

    sleep(2); // Wait for players to get ready

    // Send start signal to all players
    for (int i = 0; i < config.NUM_PLAYERS/2; i++) {
        kill(players_teamA[i].pid, SIGUSR2);
        kill(players_teamB[i].pid, SIGUSR2);
    }

    free(bin_path);

    while (1) {}

    return 0;

}

void fork_players(Player *players, int num_players, Team team, char *binary_path) {

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
            generate_random_player(&players[i], &config, team, pid, i);
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


