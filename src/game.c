//
// Created by - on 3/26/2025.
//

#include "game.h"


void init_game(Game *game) {
    game->round_num = 0;
    game->game_running = 1;
    game->team_wins_A = 0;
    game->team_wins_B = 0;
    game->last_winner = -1;
    game->elapsed_time = 0;
    game->round_running = 1;
    game->round_score = 0;
    game->total_score = 0;
}


Team simulate_round(int pipe_fds_team_A[][2], int pipe_fds_team_B[][2], Config *config, Game *game) {

    float totals_A = 0, totals_B = 0;

    for (int i = 0; i < config->NUM_PLAYERS/2; i++) {
        float effort;
        ssize_t bytes = read(pipe_fds_team_A[i][0], &effort, sizeof(float));


        if (bytes == sizeof(float) || bytes == 0) {
            printf("Team A - Player %d effort: %.2f\n", i, effort);
            totals_A += effort;
        }
    }

    for (int i = 0; i < config->NUM_PLAYERS/2; i++) {
        float effort;
        ssize_t bytes = read(pipe_fds_team_B[i][0], &effort, sizeof(float));
        if (bytes == sizeof(float) || bytes == 0) {
            printf("Team B - Player %d effort: %.2f\n", i, effort);
            totals_B += effort;
        }
    }

    game->round_score = totals_A - totals_B;
    printf("\nTotal Effort A: %.2f | Total Effort B: %.2f | Score: %.2f\n\n", totals_A, totals_B, game->round_score);

    if (game->round_score >= config->WINNING_THRESHOLD) {
        printf("🏆 Team A wins!\n");
        game->team_wins_A++;
        return TEAM_A;
    }
    if (game->round_score <= -config->WINNING_THRESHOLD) {
        printf("🏆 Team B wins!\n");
        game->team_wins_B++;
        return TEAM_B;
    }

    game->round_running = check_round_conditions(game, config);

    if (!game->round_running && game->total_score > 0) {
        printf("🏆 Team A wins!\n");
        game->team_wins_A++;
        return TEAM_A;
    }
    if (!game->round_running && game->total_score < 0) {
        printf("🏆 Team B wins!\n");
        game->team_wins_B++;
        return TEAM_B;
    }

    printf("Round ended in a draw\n");
    return -1;
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

    return 1;
}

int check_round_conditions(Game *game, Config *config) {
    if (game->elapsed_time > config->MAX_ROUND_TIME) {
        return 0;
    }
    return 1;
}

void go_to_next_round(Game *game) {
    game->round_num++;
    game->elapsed_time = 0;
    game->round_score = 0;
    game->round_running = 1;
    game->round_time = 0;
}
