//
// Created by - on 3/23/2025.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <unistd.h>

typedef enum Team {
    TEAM_A,
    TEAM_B,
    NONE
} Team;

typedef enum State {
    IDLE,
    READY,
    PULLING,
    RECOVERING
} State;

typedef struct __attribute__((packed)) {
    float rate_decay;
    float energy;
    float inital_energy;
    float recovery_time;
    float falling_chance;
} Attributes;

typedef struct Player {
    int number;
    int position;
    int new_position;
    Team team;
    Attributes attributes;
    State state;
    pid_t pid;
} Player;



Player *create_player(pid_t pid);

#endif //PLAYER_H
