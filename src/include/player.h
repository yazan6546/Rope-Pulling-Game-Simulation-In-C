#ifndef PLAYER_H
#define PLAYER_H

#include "config.h"

typedef enum {
    IDLE,
    READY,
    PULLING,
    RECOVERING
} PlayerState;

typedef struct {
    int number;
    Team team;
    float energy;
    float rate_decay;
    float recovery_time;
    PlayerState state;
} Player;

Player* create_player(int number, Team team, float energy, float rate_decay, float recovery_time);
void deserialize_player(Config *config, char *buffer);

#endif // PLAYER_H