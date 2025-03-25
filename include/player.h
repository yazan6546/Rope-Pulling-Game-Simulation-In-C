//
// Created by - on 3/23/2025.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <unistd.h>

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
    float falling_chance;
    int positon;
    State state;
    pid_t pid;
} Player;

void print_player(Player *player);
#endif //PLAYER_H
