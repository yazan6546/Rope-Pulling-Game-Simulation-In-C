//
// Created by - on 3/26/2025.
//

#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include "player.h"
#include "config.h"
#include <stdarg.h>

typedef struct Game {
    int round_num;
    int game_running;
    int round_running;
    int team_wins_A;
    int team_wins_B;
    Team last_winner;
    int elapsed_time;
    int round_time;
    float round_score;
    float total_score;
} Game;

void init_game(Game *game);
Team simulate_round(int pipe_fds_team_A[], int pipe_fds_team_B[], Config *config, Game *game);
int check_game_conditions(Game *game, Config *config, Team team_win);
void go_to_next_round(Game *game);
int check_round_conditions(Game *game, Config *config);
void print_with_time(const Game *game, const char *format, ...);

#endif //GAME_H
