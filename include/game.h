//
// Created by - on 3/26/2025.
//

#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include "player.h"
#include "config.h"
#include <stdarg.h>

#define TEAM_SIZE 4

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
    float total_effort_A;
    float total_effort_B;
    int round_time;
    int reset_round_time_flag;

    Player players_teamA[TEAM_SIZE];
    Player players_teamB[TEAM_SIZE];
} Game;

void init_game(Game *game);
Team simulate_round(int pipe_fds_team_A[], int pipe_fds_team_B[], const Config *config, Game *game,
    const Player *players_teamA, const Player *players_teamB);
int check_game_conditions(const Game *game, const Config *config, Team team_win);
void go_to_next_round(Game *game);
int check_round_conditions(const Game *game, const Config *config);
void print_with_time1(const Game *game, const char *format, ...);

#endif //GAME_H
