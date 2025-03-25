//
// Created by - on 3/26/2025.
//

#include "game.h"


void init_game(Game *game) {
    game->round_num = 0;
    game->game_running = 1;
    game->team_wins_A = 0;
    game->team_wins_B = 0;
    game->totals_A = 0;
    game->totals_B = 0;
}


Team simulate_round(int pipe_fds_team_A[][2], int pipe_fds_team_B[][2], Config *config, Game *game) {

    for (int i = 0; i < config->NUM_PLAYERS/2; i++) {
        float effort;
        ssize_t bytes = read(pipe_fds_team_A[i][0], &effort, sizeof(float));


        if (bytes == sizeof(float) || bytes == 0) {
            printf("Team A - Player %d effort: %.2f\n", i, effort);
            game->totals_A += effort;
        }
    }

    for (int i = 0; i < config->NUM_PLAYERS/2; i++) {
        float effort;
        ssize_t bytes = read(pipe_fds_team_B[i][0], &effort, sizeof(float));
        if (bytes == sizeof(float) || bytes == 0) {
            printf("Team B - Player %d effort: %.2f\n", i, effort);
            game->totals_A += effort;
        }
    }

    float score = game->totals_A - game->totals_B;
    printf("\nTotal Effort A: %.2f | Total Effort B: %.2f | Score: %.2f\n\n", game->totals_A, game->totals_B, score);

    if (score >= config->MAX_SCORE) {
        printf("🏆 Team A wins!\n");
        return TEAM_A;
    }
    if (score <= -config->MAX_SCORE) {
        printf("🏆 Team B wins!\n");
        return TEAM_B;
    }
    return -1;
}

int check_game_conditions(int round_num, Config *config) {
    if (round_num > config->NUM_ROUNDS) {
        return 0;
    }
}

