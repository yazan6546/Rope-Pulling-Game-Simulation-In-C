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
#include <sys/mman.h>

#define READ 0
#define WRITE 1
#define PATH_MAX 4096
Config config;
Game *game;

void fork_players(Player *players, int num_players, Team team, char *binary_path, int pipe_fds[][2]);
void generate_and_align(Player *players, int num_players, Team team);
void print_with_time(const char *format, ...);

void handle_sigusr1(int sig) {
    // Empty handler just to prevent termination
}

int main(int argc, char *argv[]) {

    printf(argv[1]);
    printf("\n");

    // In child
    int fd = atoi(argv[1]);
    game = mmap(NULL, sizeof(Game), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (game == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }
    char *config_path = NULL;
    handling_file(argc, argv[0], &config_path);
    char *bin_path = binary_dir(config_path);


    if (load_config("../config.txt", &config) == -1) {
        free(bin_path);
        return 1;
    }

    Player players_teamA[config.NUM_PLAYERS/2];
    Player players_teamB[config.NUM_PLAYERS/2];


    int pipe_fds_team_A[config.NUM_PLAYERS/2][2];
    int pipe_fds_team_B[config.NUM_PLAYERS/2][2];


    generate_and_align(players_teamA, config.NUM_PLAYERS/2, TEAM_A);
    generate_and_align(players_teamB, config.NUM_PLAYERS/2, TEAM_B);

    fork_players(players_teamA, config.NUM_PLAYERS/2, TEAM_A, bin_path, pipe_fds_team_A);
    fork_players(players_teamB, config.NUM_PLAYERS/2, TEAM_B, bin_path, pipe_fds_team_B);

    // Set up signal handlers
    signal(SIGUSR1, handle_sigusr1);
    Team team_win = NONE;

    while (game->game_running) {

        printf("\n\n");

        // Send get ready signal to all players
        for (int i = 0; i < config.NUM_PLAYERS/2; i++) {
            kill(players_teamA[i].pid, SIGUSR1);
            kill(players_teamB[i].pid, SIGUSR1);
        }

        printf("signal SIGUSR1 sent to all players\n");
        fflush(stdout);

        sleep(2);

        printf("\n\n");

        //
        // Send start signal to all players
        for (int i = 0; i < config.NUM_PLAYERS/2; i++) {
            kill(players_teamA[i].pid, SIGUSR2);
            kill(players_teamB[i].pid, SIGUSR2);
        }
        printf("\n\n");


        while (game->round_running) {

            team_win = simulate_round(pipe_fds_team_A, pipe_fds_team_B,
                                                        &config, game);

            if (team_win != NONE) {
                game->round_running = 0;
            }
            printf("ok\n");
            fflush(stdout);
            printf("aaa\n");
            fflush(stdout);

        }

        game->total_score += game->round_score;
        go_to_next_round(game);
        game->game_running = check_game_conditions(game , &config, team_win);
        game->last_winner = team_win;
        team_win = NONE;
    }

    printf("Team A wins: %d\n", game->team_wins_A);
    printf("Team B wins: %d\n\n", game->team_wins_B);

    printf("Cleaning up...\n");

    // free(bin_path);
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

            if (execl("./player", "player", buffer, write_fd_str, read_fd_str, &game->elapsed_time, NULL)) {
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

void print_with_time(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("@ %ds: ", game->elapsed_time);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
}

