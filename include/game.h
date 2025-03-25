//
// Created by - on 3/26/2025.
//

#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include "player.h"
#include "config.h"

typedef struct Game {
    int round_num;
    int game_running;
    int team_wins_A;
    int team_wins_B;
    float totals_A;
    float totals_B;
} Game;

void init_game(Game *game);
Team simulate_round(int pipe_fds_team_A[][2], int pipe_fds_team_B[][2], Config *config, Game *game);
int check_game_conditions(int round_num, Config *config);


#endif //GAME_H
