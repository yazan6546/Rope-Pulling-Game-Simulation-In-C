#include "common.h"
#include "config.h"
#include "random.h"
#include <signal.h>
#include <wchar.h>
#include "referee_orders.h"
#include "player_utils.h"
#include "player.h"
#include "game.h"
#include <stdarg.h>
#include <sys/mman.h>


#define PATH_MAX 4096
Config config;
Game *game;

void fork_players(Player *players, int num_players, Team team, int read_fds[], int pos_pipe_fds[], int fd);
void generate_and_align(Player *players, int num_players, Team team, int *read_fds, int *pos_pipe_fds);
void cleanup_processes(const Player *players_teamA, const Player *players_teamB, int NUM_PLAYERS);
void print_with_time(const char *format, ...);
void send_new_positions(Player *players, int num_players, int pos_pipe_fds[]);
void read_player_energies(Player *players, int num_players, int pos_pipe_fds[]);
void change_player_positions(Player *player, int num_players);
void handle_new_round(Game *game, int *read_fds_team_A, int *read_fds_team_B,
                      int *pos_pipe_fds_team_A, int *pos_pipe_fds_team_B);

volatile int elapsed_time = 0;
int gui_pid;


int main(int argc, char *argv[]) {
    

    if (argc < 4) {
        fprintf(stderr, "Usage: referee <config buffer> <fd> <gui_pid>\n");
        exit(EXIT_FAILURE);
    }
    printf("%s", argv[2]);
    printf("\n");

    init_random(getpid());
    // In child
    int fd = atoi(argv[2]);
    game = mmap(NULL, sizeof(Game), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (game == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    char *buffer = argv[1];
    // Deserialize the config
    deserialize_config(&config, buffer);

    // Store GUI’s PID from argv[2] in game structure (or local variable)
    gui_pid = atoi(argv[3]);

    int read_fds_team_A[config.NUM_PLAYERS/2];
    int read_fds_team_B[config.NUM_PLAYERS/2];
    int pos_pipe_fds_team_A[config.NUM_PLAYERS/2];
    int pos_pipe_fds_team_B[config.NUM_PLAYERS/2];


    generate_and_align(game->players_teamA, config.NUM_PLAYERS/2, TEAM_A, read_fds_team_A, pos_pipe_fds_team_A);
    generate_and_align(game->players_teamB, config.NUM_PLAYERS/2, TEAM_B, read_fds_team_B, pos_pipe_fds_team_B);


    fork_players(game->players_teamA, config.NUM_PLAYERS/2, TEAM_A, read_fds_team_A, pos_pipe_fds_team_A, fd);
    fork_players(game->players_teamB, config.NUM_PLAYERS/2, TEAM_B, read_fds_team_B, pos_pipe_fds_team_B, fd);

    change_player_positions(game->players_teamA, config.NUM_PLAYERS/2);
    change_player_positions(game->players_teamB, config.NUM_PLAYERS/2);

    sleep(2);

    printf("after fork\n");
    // Set up signal handlers
    // signal(SIGUSR1, handle_sigusr1);
    Team team_win = NONE;

    while (game->game_running) {

        printf("\n\n");

        // Send get ready signal to all players
        for (int i = 0; i < config.NUM_PLAYERS/2; i++) {
            kill(game->players_teamA[i].pid, SIGUSR1);
            kill(game->players_teamB[i].pid, SIGUSR1);
        }

        // Send READY signal to the GUI
        kill(gui_pid, SIGUSR1);

        printf("signal SIGUSR1 sent to all players\n");
        fflush(stdout);

        // Wait for players to get ready
        sleep(2);

        printf("\n\n");

        //
        // Send start signal to all players
        for (int i = 0; i < config.NUM_PLAYERS/2; i++) {
            kill(game->players_teamA[i].pid, SIGUSR2);
            kill(game->players_teamB[i].pid, SIGUSR2);
        }

        // Send START signal to the GUI
        kill(gui_pid, SIGUSR2);

        printf("\n\n");

        sleep(1); // Wait for players to start

        game->reset_round_time_flag = 0; // Reset the flag for the next round
        while (game->round_running) {

            team_win = simulate_round(read_fds_team_A, read_fds_team_B,
                                        &config, game);


            if (team_win != NONE) {
                game->round_running = 0;
                game->reset_round_time_flag = 1;
            }

        }

        usleep(50000); // Sleep for 5 seconds to simulate round time

        game->total_score += game->round_score;
        game->game_running = check_game_conditions(game , &config, team_win);

        if (game->game_running) {
            handle_new_round(game, read_fds_team_A, read_fds_team_B,
                             pos_pipe_fds_team_A, pos_pipe_fds_team_B);
        }

        sleep(2);

        game->last_winner = team_win;
        team_win = NONE;
    }

    printf("Team A wins: %d\n", game->team_wins_A);
    printf("Team B wins: %d\n\n", game->team_wins_B);

    printf("Cleaning up...\n");

    cleanup_processes(game->players_teamA, game->players_teamB, config.NUM_PLAYERS);
    // free(bin_path);
    return 0;

}

void fork_players(Player *players, int num_players, Team team,
                    int read_fds[], int pos_pipe_fds[], int fd) {

    for (int i = 0; i < num_players; i++) {

        int energy_pipe_fds[2];
        if (pipe(energy_pipe_fds) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        read_fds[players[i].number] = energy_pipe_fds[0];

        int pos_pipe_fds_temp[2];
        if (pipe(pos_pipe_fds_temp) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        pos_pipe_fds[players[i].number] = pos_pipe_fds_temp[1];  // Parent writes on this end later

        const pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            fflush(stderr);
            exit(EXIT_FAILURE);
        }
        if (pid == 0) { // Child process

            char buffer[100];
            serialize_player(&players[i], buffer);

            char energy_fd_str[10], pos_fd_str[10], fd_str[10];
            snprintf(energy_fd_str, sizeof(energy_fd_str), "%d", energy_pipe_fds[1]);
            snprintf(pos_fd_str, sizeof(pos_fd_str), "%d", pos_pipe_fds_temp[0]);
            snprintf(fd_str, sizeof(fd_str), "%d", fd);

            if (execl("./player", "player", buffer, energy_fd_str, pos_fd_str, fd_str, NULL)) {
                perror("execl");
                printf("Child process %d failed to exec and will terminate\n", getpid());
                // Sleep briefly to allow output to be written
                usleep(100000);  // 100ms delay
                exit(1);
            }
        } else { // Parent process
            players[i].pid = pid;
            close(energy_pipe_fds[1]); // Close the write end of the pipe in the parent
            close(pos_pipe_fds_temp[0]); // Close read end for dedicated pos pipe in parent
        }
    }
}

void generate_and_align(Player *players, int num_players, Team team, int *read_fds, int *pos_pipe_fds) {
    for (int i = 0; i<num_players; i++) {
        generate_random_player(&players[i], &config, team, i);
    }

    align(players, num_players, read_fds, pos_pipe_fds);
}

void print_with_time(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("@ %ds: ", game->elapsed_time);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
}


// Cleanup function to kill all child processes
void cleanup_processes(const Player *players_teamA, const Player *players_teamB, int NUM_PLAYERS) {
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

void send_new_positions(Player *players, int num_players, int pos_pipe_fds[]) {
    for (int i = 0; i < num_players; i++) {
        int index_pipe = players[i].number;
        if (write(pos_pipe_fds[index_pipe], &players[i].new_position, sizeof(int)) <=0) {
            perror("write to pos pipe");
            fflush(stderr);
            usleep(10000);
            // Sleep briefly to allow output to be written
            exit(1);
        }
    }
}

void read_player_energies(Player *players, int num_players, int pos_pipe_fds[]) {
    for (int i = 0; i < num_players; i++) {
        float energy;
        int index_pipe = players[i].number;
        if (read(pos_pipe_fds[index_pipe], &energy, sizeof(float)) <= 0) {
            perror("read from energy pipe");
        }

        // Update player energy
        players[i].attributes.energy = energy;
    }
}

void change_player_positions(Player *player, int num_players) {
    for (int i = 0; i < num_players; i++) {
        player[i].position = player[i].new_position;
    }
}


void handle_new_round(Game *game, int *read_fds_team_A, int *read_fds_team_B,
                      int *pos_pipe_fds_team_A, int *pos_pipe_fds_team_B) {

    // send reset signals to all players
    go_to_next_round(game);

    for (int i = 0; i < config.NUM_PLAYERS/2; i++) {
        kill(game->players_teamA[i].pid, SIGHUP);
        kill(game->players_teamB[i].pid, SIGHUP);
    }

    sleep(1); // Wait for all players to reset

    read_player_energies(game->players_teamA, config.NUM_PLAYERS/2, read_fds_team_A);
    read_player_energies(game->players_teamB, config.NUM_PLAYERS/2, read_fds_team_B);


    align(game->players_teamA, config.NUM_PLAYERS/2, read_fds_team_A, pos_pipe_fds_team_A);
    align(game->players_teamB, config.NUM_PLAYERS/2, read_fds_team_B, pos_pipe_fds_team_B);

    printf("\n\n");
    // After resetting rounds, send new positions through pipes
    send_new_positions(game->players_teamA, config.NUM_PLAYERS/2, pos_pipe_fds_team_A);
    send_new_positions(game->players_teamB, config.NUM_PLAYERS/2, pos_pipe_fds_team_B);

    change_player_positions(game->players_teamA, config.NUM_PLAYERS/2);
    change_player_positions(game->players_teamB, config.NUM_PLAYERS/2);
}





