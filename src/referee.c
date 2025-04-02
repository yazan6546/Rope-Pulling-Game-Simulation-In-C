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
#include <stdarg.h>

#define PATH_MAX 4096
Config config;

void fork_players(Player *players, int num_players, Team team, char *binary_path, int read_fds[]);
void generate_and_align(Player *players, int num_players, Team team);
void handle_alarm(int signum);
void cleanup_processes(Player *players_teamA, Player *players_teamB, int NUM_PLAYERS);

volatile int elapsed_time = 0;

int main(int argc, char *argv[]) {

    Game game;
    init_game(&game);

    char *config_path = NULL;
    handling_file(argc, argv[0], &config_path);
    char *bin_path = binary_dir(config_path);


    if (load_config(config_path, &config) == -1) {
        free(bin_path);
        return 1;
    }

    Player players_teamA[config.NUM_PLAYERS/2];
    Player players_teamB[config.NUM_PLAYERS/2];


    int read_fds_team_A[config.NUM_PLAYERS/2];
    int read_fds_team_B[config.NUM_PLAYERS/2];


    generate_and_align(players_teamA, config.NUM_PLAYERS/2, TEAM_A);
    generate_and_align(players_teamB, config.NUM_PLAYERS/2, TEAM_B);

    fork_players(players_teamA, config.NUM_PLAYERS/2, TEAM_A, bin_path, read_fds_team_A);
    fork_players(players_teamB, config.NUM_PLAYERS/2, TEAM_B, bin_path, read_fds_team_B);


    Team team_win = NONE;

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

        while (game.round_running) {

            team_win = simulate_round(read_fds_team_A, read_fds_team_B,
                                                        &config, &game);

            sleep(1);

            game.elapsed_time++;
            game.round_time++;

            if (team_win != NONE) {
                game.round_running = 0;
            }

        }

        game.total_score += game.round_score;
        go_to_next_round(&game);
        game.game_running = check_game_conditions(&game , &config, team_win);
        game.last_winner = team_win;
        team_win = NONE;
    }

    printf("Team A wins: %d\n", game.team_wins_A);
    printf("Team B wins: %d\n\n", game.team_wins_B);

    printf("Cleaning up...\n");

    cleanup_processes(players_teamA, players_teamB, config.NUM_PLAYERS);
    // free(bin_path);
    return 0;

}

void fork_players(Player *players, int num_players, Team team,
                char *binary_path, int read_fds[]) {

    for (int i = 0; i < num_players; i++) {

        int pipe_fds_temp[2];
        if (pipe(pipe_fds_temp) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        read_fds[i] = pipe_fds_temp[0];

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
            snprintf(write_fd_str, sizeof(write_fd_str), "%d", pipe_fds_temp[1]);

            if (execl(player_path, "player", buffer, write_fd_str, NULL)) {
                perror("execl");
                exit(1);
            }
        } else { // Parent process
            players[i].pid = pid;
            close(pipe_fds_temp[1]); // Close the write end of the pipe in the parent
        }
    }
}

void generate_and_align(Player *players, int num_players, Team team) {

    for (int i = 0; i<num_players; i++) {
        generate_random_player(&players[i], &config, team, i);
    }

    align(players, num_players);
}

void handle_alarm(int signum) {
    elapsed_time++;
    alarm(1);
}


// Cleanup function to kill all child processes
void cleanup_processes(Player *players_teamA, Player *players_teamB, int NUM_PLAYERS) {
    printf("Killing all child processes...\n");

    // Kill players from Team A
    for (int i = 0; i < NUM_PLAYERS; i++) {
        if (players_teamA[i].pid > 0) {
            kill(players_teamA[i].pid, SIGKILL);
        }
    }

    // Kill players from Team B
    for (int i = 0; i < NUM_PLAYERS; i++) {
        if (players_teamB[i].pid > 0) {
            kill(players_teamB[i].pid, SIGKILL);
        }
    }

    printf("All child processes terminated.\n");
}






