//
// Created by - on 3/23/2025.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>

typedef enum Team {
    TEAM_A,
    TEAM_B
} Team;

typedef enum State {
    IDLE,
    READY,
    PULLING,
    RECOVERING
} State;

typedef struct Player {
    int number;
    Team team;
    float energy;
    float rate_decay;
    float recovery_time;
    State state;
} Player;

void print_player(Player *player);
#endif //PLAYER_H
