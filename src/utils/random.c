//
// Created by - on 3/23/2025.
//

#include "random.h"

void init_random(pid_t pid) {
    srand( (unsigned int) pid + time(NULL) );
}

float random_float(float min, float max) {
    return rand() / (float)RAND_MAX  *  (max - min) + min;
}

void generate_random_player(Player *player, Config *configs, Team team, int number) {
    randomize_attributes(&(player->attributes), configs);
    player->team = team;
    player->number = number;
    player->position = number+1;
    player->new_position = number+1;
}

void randomize_attributes(Attributes *attributes, Config *configs) {
    attributes->rate_decay = random_float(configs->MIN_RATE_DECAY, configs->MAX_RATE_DECAY);
    attributes->energy = random_float(configs->MIN_ENERGY, configs->MAX_ENERGY);
    attributes->recovery_time = random_float(configs->MIN_RECOVERY_TIME, configs->MAX_RECOVERY_TIME);
    attributes->falling_chance = random_float(configs->MIN_FALLING_CHANCE, configs->MAX_FALLING_CHANCE);
}