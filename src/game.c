//
// Created by - on 3/26/2025.
//

#include "game.h"

#include <message.h>
#include <string.h>


void init_game(Game *game) {
    game->round_time = 0;
    game->elapsed_time = 0;
    game->total_effort_A = 0;
    game->total_effort_B = 0;
    game->round_num = 1;
    game->game_running = 1;
    game->team_wins_A = 0;
    game->team_wins_B = 0;
    game->last_winner = NONE;
    game->elapsed_time = 0;
    game->round_running = 1;
    game->round_score = 0;
    game->total_score = 0;
    game->reset_round_time_flag = 1; // Initialize the flag
}


Team simulate_round(int pipe_fds_team_A[], int pipe_fds_team_B[], const Config *config, Game *game) {
    float total_effort_A = 0, total_effort_B = 0;
    char output_buffer[4096] = "";  // Large buffer for all output
    char temp_buffer[256];          // Temporary buffer for formatting

    strcat(output_buffer, "\n");

    // Process Team A efforts
    for (int i = 0; i < config->NUM_PLAYERS/2; i++) {
        Message message;
        int index_pipe = game->players_teamA[i].number;
        ssize_t bytes = read(pipe_fds_team_A[index_pipe], &message, sizeof(Message));

        if (bytes == sizeof(Message) || bytes == 0) {
            snprintf(temp_buffer, sizeof(temp_buffer),
                     "Team A - Player %d effort: %.2f\n", game->players_teamA[i].number, message.effort);
            strcat(output_buffer, temp_buffer);
            total_effort_A += message.effort;
            game->players_teamA[i].attributes.energy = message.effort / game->players_teamA[i].position;
            game->players_teamA[i].state = message.state;
        }
        else {
            perror("read");
            fflush(stderr);
            usleep(10000);
            // Sleep briefly to allow output to be written
            exit(1);
        }

    }

    // Process Team B efforts
    for (int i = 0; i < config->NUM_PLAYERS/2; i++) {
        Message message;
        int index_pipe = game->players_teamB[i].number;
        ssize_t bytes = read(pipe_fds_team_B[index_pipe], &message, sizeof(Message));

        if (bytes == sizeof(Message) || bytes == 0) {
            snprintf(temp_buffer, sizeof(temp_buffer),
                     "Team B - Player %d effort: %.2f\n", game->players_teamB[i].number, message.effort);
            strcat(output_buffer, temp_buffer);
            total_effort_B += message.effort;
            game->players_teamB[i].attributes.energy = message.effort / game->players_teamB[i].position;
            game->players_teamB[i].state = message.state;
        }
        else {
            perror("read");
            fflush(stderr);
            usleep(10000);
            // Sleep briefly to allow output to be written
            exit(1);
        }
    }

    // Calculate round score and add totals
    game->round_score = total_effort_A - total_effort_B;
    game->total_effort_A = total_effort_A;
    game->total_effort_B = total_effort_B;
    snprintf(temp_buffer, sizeof(temp_buffer),
             "\nTotal Effort A: %.2f | Total Effort B: %.2f | Score: %.2f\n\n",
             total_effort_A, total_effort_B, game->round_score);
    strcat(output_buffer, temp_buffer);

    // Determine winner
    Team winner = NONE;

    if (game->round_score >= config->WINNING_THRESHOLD) {
        strcat(output_buffer, "🏆 Team A wins!\n");
        strcat(output_buffer, "Round score exceeded threshold!\n");
        game->team_wins_A++;
        winner = TEAM_A;
    }
    else if (game->round_score <= -config->WINNING_THRESHOLD) {
        strcat(output_buffer, "🏆 Team B wins!\n");
        strcat(output_buffer, "Round score exceeded threshold!\n");
        game->team_wins_B++;
        winner = TEAM_B;
    }
    else if (game->round_time > config->MAX_ROUND_TIME) {
        if (game->round_score > 0) {

            strcat(output_buffer, "🏆 Team A wins!\n");
            game->team_wins_A++;
            winner = TEAM_A;
        }
        else if (game->round_score < 0) {
            strcat(output_buffer, "🏆 Team B wins!\n");
            game->team_wins_B++;
            winner = TEAM_B;
        }
        else {
            strcat(output_buffer, "It's a draw!\n");
        }

        strcat(output_buffer, "Round Time is up!\n");
    }
    else if (game->elapsed_time > config->MAX_TIME) {
        if (game->round_score > 0) {
            strcat(output_buffer, "🏆 Team A wins!\n");
            game->team_wins_A++;
            winner = TEAM_A;
        } else if (game->round_score < 0) {
            strcat(output_buffer, "🏆 Team B wins!\n");
            game->team_wins_B++;
            winner = TEAM_B;
        }
        else {
            strcat(output_buffer, "It's a draw!\n");
        }

        strcat(output_buffer, "Game time is up!\n");
    }

    // Print everything at once with timestamp
    print_with_time1(game, "%s", output_buffer);
    fflush(stdout);

    return winner;
}

int check_game_conditions(const Game *game, const Config *config, Team team_win) {
    if (game->round_num >= config->NUM_ROUNDS) {
        printf("NUM ROUNDS\n");
        fflush(stdout);
        return 0;
    }

    if (team_win == game->last_winner && team_win != -1) {
        printf("CONSECUTIVE WINS\n");
        fflush(stdout);
        return 0;
    }

    if (game->elapsed_time > config->MAX_TIME) {
        printf("MAX TIME\n");
        fflush(stdout);
        return 0;
    }

    if (game->total_score > config->MAX_SCORE) {
        printf("MAX SCORE\n");
        fflush(stdout);
        return 0;
    }

    if (game->total_score > config->MAX_SCORE) {
        fflush(stdout);
        return 0;
    }
    return 1;
}

int check_round_conditions(const Game *game, const Config *config) {
    if (game->round_time > config->MAX_ROUND_TIME) {
        return 0;
    }

    if (game->round_score >= config->WINNING_THRESHOLD || game->round_score <= -config->WINNING_THRESHOLD) {
        return 0;
    }
    return 1;
}

void go_to_next_round(Game *game) {
    game->round_num++;
    game->round_score = 0;
    game->round_running = 1;
    game->reset_round_time_flag = 1; // Reset round time
}


void print_with_time1(const Game *game, const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("@ g:%ds r:%ds: ", game->elapsed_time, game->round_time);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
}
