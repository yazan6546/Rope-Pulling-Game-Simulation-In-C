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
    Team last_winner;
} Game;

void init_game(Game *game);
Team simulate_round(int pipe_fds_team_A[][2], int pipe_fds_team_B[][2], Config *config, Game *game);
int check_game_conditions(Game *game, Config *config, Team team_win);
void go_to_next_round(Game *game);


#endif //GAME_H
