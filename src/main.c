#include "common.h"
#include "config.h"
#include "random.h"
#include <signal.h>
#include <wchar.h>
#include "referee_orders.h"
#include "player_utils.h"
#include "file.h"
#include "player.h"
#include "game.h"

#define READ 0
#define WRITE 1
#define PATH_MAX 4096
Config config;

void fork_players(Player *players, int num_players, Team team, char *binary_path, int pipe_fds[][2]);
void generate_and_align(Player *players, int num_players, Team team);

int main(int argc, char *argv[]) {

    Game game;
    init_game(&game);

    char *config_path = NULL;
    handling_file(argc, argv[0], &config_path);
    char *bin_path = binary_dir(config_path);


    load_config(config_path, &config);

    Player players_teamA[config.NUM_PLAYERS/2];
    Player players_teamB[config.NUM_PLAYERS/2];


    int pipe_fds_team_A[config.NUM_PLAYERS/2][2];
    int pipe_fds_team_B[config.NUM_PLAYERS/2][2];


    generate_and_align(players_teamA, config.NUM_PLAYERS/2, TEAM_A);
    generate_and_align(players_teamB, config.NUM_PLAYERS/2, TEAM_B);

    fork_players(players_teamA, config.NUM_PLAYERS/2, TEAM_A, bin_path, pipe_fds_team_A);
    fork_players(players_teamB, config.NUM_PLAYERS/2, TEAM_B, bin_path, pipe_fds_team_B);

    Team team_win = -1;

    while (game.game_running) {
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

        while (game.game_running && game.round_running) {

            team_win = simulate_round(pipe_fds_team_A, pipe_fds_team_B,
                                                        &config, &game);

            sleep(1);

            game.elapsed_time++;
            game.round_time++;
            game.game_running = check_game_conditions(&game , &config, team_win);
        }

        game.total_score += game.round_score;
        go_to_next_round(&game);
        game.game_running = check_game_conditions(&game , &config, team_win);
        game.last_winner = team_win;
        team_win = -1;
    }

    printf("Team A wins: %d\n", game.team_wins_A);
    printf("Team B wins: %d\n\n", game.team_wins_B);

    printf("Cleaning up...\n");

    free(bin_path);
    return 0;

}

void fork_players(Player *players, int num_players, Team team,
                char *binary_path, int pipe_fds[][2]) {

    for (int i = 0; i < num_players; i++) {
        if (pipe(pipe_fds[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        const pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) { // Child process

            char buffer[100];
            serialize_player(&players[i], buffer);

            char player_path[PATH_MAX];
            snprintf(player_path, PATH_MAX, "%s/player", binary_path);

            char write_fd_str[10], read_fd_str[10];
            sprintf(write_fd_str, "%d", pipe_fds[i][WRITE]);
            sprintf(read_fd_str, "%d", pipe_fds[i][READ]);

            if (execl(player_path, "player", buffer, write_fd_str, read_fd_str, NULL)) {
                perror("execl");
                exit(1);
            }
        } else { // Parent process
            players[i].pid = pid;

        }
    }
}

void generate_and_align(Player *players, int num_players, Team team) {

    for (int i = 0; i<num_players; i++) {
        generate_random_player(&players[i], &config, team, i);
    }

    align(players, num_players);
}


