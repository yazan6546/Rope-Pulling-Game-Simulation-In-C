//
// Created by - on 3/26/2025.
//

#include "game.h"


void init_game(Game *game) {
    game->round_num = 0;
    game->game_running = 1;
    game->team_wins_A = 0;
    game->team_wins_B = 0;
    game->last_winner = NONE;
    game->elapsed_time = 0;
    game->round_running = 1;
    game->round_score = 0;
    game->total_score = 0;
    game->reset_round_time_flag = 0;
}


Team simulate_round(int pipe_fds_team_A[], int pipe_fds_team_B[], Config *config, Game *game,
    Player *players_teamA, Player *players_teamB) {
    float totals_A = 0, totals_B = 0;
    char output_buffer[4096] = "";  // Large buffer for all output
    char temp_buffer[256];          // Temporary buffer for formatting

    strcat(output_buffer, "\n");

    // Process Team A efforts
    for (int i = 0; i < config->NUM_PLAYERS/2; i++) {
        float effort;
        ssize_t bytes = read(pipe_fds_team_A[i], &effort, sizeof(float));

        if (bytes == sizeof(float) || bytes == 0) {
            snprintf(temp_buffer, sizeof(temp_buffer),
                     "Team A - Player %d effort: %.2f\n", players_teamA[i].number, effort);
            strcat(output_buffer, temp_buffer);
            totals_A += effort;
        }
    }

    // Process Team B efforts
    for (int i = 0; i < config->NUM_PLAYERS/2; i++) {
        float effort;
        ssize_t bytes = read(pipe_fds_team_B[i], &effort, sizeof(float));

        if (bytes == sizeof(float) || bytes == 0) {
            snprintf(temp_buffer, sizeof(temp_buffer),
                     "Team B - Player %d effort: %.2f\n", players_teamB[i].number, effort);
            strcat(output_buffer, temp_buffer);
            totals_B += effort;
        }
    }

    // Calculate round score and add totals
    game->round_score = totals_A - totals_B;
    snprintf(temp_buffer, sizeof(temp_buffer),
             "\nTotal Effort A: %.2f | Total Effort B: %.2f | Score: %.2f\n\n",
             totals_A, totals_B, game->round_score);
    strcat(output_buffer, temp_buffer);

    // Determine winner
    Team winner = NONE;

    if (game->round_score >= config->WINNING_THRESHOLD) {
        strcat(output_buffer, "🏆 Team A wins!\n");
        game->team_wins_A++;
        winner = TEAM_A;
    } else if (game->round_score <= -config->WINNING_THRESHOLD) {
        strcat(output_buffer, "🏆 Team B wins!\n");
        game->team_wins_B++;
        winner = TEAM_B;
    } else if (game->round_time > config->MAX_ROUND_TIME) {
        if (game->round_score > 0) {
            strcat(output_buffer, "🏆 Team A wins!\n");
            game->team_wins_A++;
            winner = TEAM_A;
        } else if (game->round_score < 0) {
            strcat(output_buffer, "🏆 Team B wins!\n");
            game->team_wins_B++;
            winner = TEAM_B;
        }
    } else if (game->elapsed_time > config->MAX_TIME) {
        if (game->round_score > 0) {
            strcat(output_buffer, "🏆 Team A wins!\n");
            game->team_wins_A++;
            winner = TEAM_A;
        } else if (game->round_score < 0) {
            strcat(output_buffer, "🏆 Team B wins!\n");
            game->team_wins_B++;
            winner = TEAM_B;
        }
    }

    // Print everything at once with timestamp
    print_with_time1(game, "%s", output_buffer);
    fflush(stdout);

    return winner;
}

int check_game_conditions(Game *game, Config *config, Team team_win) {
    if (game->round_num > config->NUM_ROUNDS) {  
        printf("NUM ROUNDS\n");
        return 0;
    }

    if (team_win == game->last_winner && team_win != -1) {
        printf("CONSECUTIVE WINS\n");
        return 0;
    }

    if (game->elapsed_time > config->MAX_TIME) {
        printf("MAX TIME\n");
        return 0;
    }

    if (game->total_score > config->MAX_SCORE) {
        printf("MAX SCORE\n");
        return 0;
    }

    if (game->total_score > config->MAX_SCORE) {
        return 0;
    }
    return 1;
}

int check_round_conditions(Game *game, Config *config) {
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
