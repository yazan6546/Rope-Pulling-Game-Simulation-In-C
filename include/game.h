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
    int team_wins_A;
    int team_wins_B;
    Team last_winner;
    int elapsed_time;
    int round_running;
    float round_score;
    float total_score;
    int round_time;
    int reset_round_time_flag; // New field for time management
} Game;

void init_game(Game *game);
Team simulate_round(int pipe_fds_team_A[], int pipe_fds_team_B[], Config *config, Game *game,
    Player *players_teamA, Player *players_teamB);
int check_game_conditions(Game *game, Config *config, Team team_win);
void go_to_next_round(Game *game);
int check_round_conditions(Game *game, Config *config);
void print_with_time1(const Game *game, const char *format, ...);

#endif //GAME_H
