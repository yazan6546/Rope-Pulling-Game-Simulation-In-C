//
// Created by - on 3/23/2025.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>
#include <stdio.h>
#include "config.h"


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
void deserialize_player(Config *config, char *buffer);
void fork_players(Team team, int num_players);


#endif //PLAYER_H
