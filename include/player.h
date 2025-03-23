//
// Created by - on 3/23/2025.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>

typedef enum Team {
    TEAM_A,
    TEAM_B
} Team;

typedef struct Player {
    int number;
    Team team;
    float energy;
    float rate_decay;
    float recovery_time;
} Player;

Player* create_player(int number, Team team, float energy, float rate_decay, float recovery_time);


#endif //PLAYER_H
