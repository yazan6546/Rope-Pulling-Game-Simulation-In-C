//
// Created by - on 3/23/2025.
//

#include "random.h"

void init_random(pid_t pid) {
    srand(pid);
}

float random_float(float min, float max) {
    return rand() / (float)RAND_MAX  *  (max - min) + min;
}

void generate_random_player(Player *player, Config *configs, Team team, int number) {
    player->rate_decay = random_float(configs->MIN_RATE_DECAY, configs->MAX_RATE_DECAY);
    player->energy = random_float(configs->MIN_ENERGY, configs->MAX_ENERGY);
    player->recovery_time = random_float(configs->MIN_RECOVERY_TIME, configs->MAX_RECOVERY_TIME);
    player->team = team;
    player->number = number;
}