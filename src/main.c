#include "common.h"
#include "config.h"
#include "random.h"
#include "player_utils.h"
#include "file.h"

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define PATH_MAX 4096
Config config;

void fork_players(Team team, int num_players, char *bin_path,float energies[]);

int main(int argc, char *argv[]) {

    char *config_path = NULL;
    handling_file(argc, argv[0], &config_path);
    char *bin_path = binary_dir(config_path);

    printf("Config path: %s\n", argv[0]);
    printf("Config path: %s\n", config_path);

    load_config(config_path, &config);
    print_config(&config);

    int num_players_per_team = config.NUM_PLAYERS / 2;

    float energies_A[num_players_per_team];
    float energies_B[num_players_per_team];

    fork_players(TEAM_A, num_players_per_team, bin_path, energies_A);
    fork_players(TEAM_B, num_players_per_team, bin_path, energies_B);

    float total_energy_A = 0.0, total_energy_B = 0.0;

    for (int i = 0; i < num_players_per_team; i++) {
        total_energy_A += energies_A[i];
        total_energy_B += energies_B[i];
    }

    int score = total_energy_A - total_energy_B;

    printf("\nTotal Energy For Team A: %.2f\n", total_energy_A);
    printf("Total Energy For Team B: %.2f\n", total_energy_B);
    printf("Current Score: %d\n", score);

    if (score > 0) {
        printf("Team A is currently winning.\n");
    } else if (score < 0) {
        printf("Team B is currently winning.\n");
    } else {
        printf("It's a tie!\n");
    }

    free(bin_path);
    return 0;
}


void fork_players(Team team, int num_players, char *binary_path, float energies[]) {
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

            float energy;
            ssize_t bytes_read = read(pipefd[0], &energy, sizeof(float));
            if (bytes_read == sizeof(float)) {
                printf("Received energy from player %d: %.2f\n", i, energy);
                energies[i] = energy;
            } else {
                printf("Failed to read energy from player %d\n", i);
                energies[i] = 0.0;
            }

            close(pipefd[0]);
        }
    }
}

