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
}


Team simulate_round(int pipe_fds_team_A[], int pipe_fds_team_B[], Config *config, Game *game) {

    float totals_A = 0, totals_B = 0;

    for (int i = 0; i < config->NUM_PLAYERS/2; i++) {
        float effort;
        ssize_t bytes = read(pipe_fds_team_A[i], &effort, sizeof(float));


        if (bytes == sizeof(float) || bytes == 0) {
            print_with_time(game, "Team A - Player %d effort: %.2f\n", i, effort);
            totals_A += effort;
        }
    }

    for (int i = 0; i < config->NUM_PLAYERS/2; i++) {
        float effort;
        ssize_t bytes = read(pipe_fds_team_B[i], &effort, sizeof(float));
        if (bytes == sizeof(float) || bytes == 0) {
            print_with_time(game, "Team B - Player %d effort: %.2f\n", i, effort);
            totals_B += effort;
        }
    }

    game->round_score = totals_A - totals_B;
    print_with_time(game, "\nTotal Effort A: %.2f | Total Effort B: %.2f | Score: %.2f\n\n", totals_A, totals_B, game->round_score);


    if (game->round_score >= config->WINNING_THRESHOLD) {
        print_with_time(game, "🏆 Team A wins!\n");
        game->team_wins_A++;
        return TEAM_A;
    }
    if (game->round_score <= -config->WINNING_THRESHOLD) {
        print_with_time(game, "🏆 Team B wins!\n");
        game->team_wins_B++;
        return TEAM_B;
    }


    if (game->round_time > config->MAX_ROUND_TIME && game->round_score > 0) {
        print_with_time(game, "🏆 Team A wins!\n");
        game->team_wins_A++;
        return TEAM_A;
    }
    if (game->round_time > config->MAX_ROUND_TIME && game->round_score < 0) {
        print_with_time(game, "🏆 Team B wins!\n");
        game->team_wins_B++;
        return TEAM_B;
    }

    if (game->elapsed_time > config->MAX_TIME && game->round_score > 0) {
        print_with_time(game, "🏆 Team A wins!\n");
        game->team_wins_A++;
        return TEAM_A;
    }

    if (game->elapsed_time > config->MAX_TIME && game->round_score < 0) {
        print_with_time(game, "🏆 Team B wins!\n");
        game->team_wins_B++;
        return TEAM_B;
    }

    return NONE;
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
    game->round_time = 0;
}


void print_with_time(const Game *game, const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("@ g:%ds r:%ds: ", game->elapsed_time, game->round_time);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
}
