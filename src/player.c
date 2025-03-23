//
// Created by - on 3/23/2025.
//

#include "player.h"

int main(int argc, char *argv[]) {


    printf("Player process\n");
    printf("argv[1] = %s\n", argv[1]);
    Config config;
    deserialize_player(&config, argv[1]);

    fflush(stdout);

    return 0;
}

Player* create_player(int number, Team team, float energy, float rate_decay, float recovery_time) {
    Player* player = malloc(sizeof(Player));
    player->number = number;
    player->team = team;
    player->energy = energy;
    player->rate_decay = rate_decay;
    player->recovery_time = recovery_time;
    return player;
}

void deserialize_player(Config *config, char *buffer) {
    sscanf(buffer, "%f %f %f %f %f %f %f %f %f %f %d", &config->MIN_RATE_DECAY, &config->MAX_RATE_DECAY,
            &config->MIN_ENERGY, &config->MAX_ENERGY, &config->MAX_SCORE,
            &config->MAX_TIME, &config->NUM_ROUNDS, &config->MIN_RECOVERY_TIME,
            &config->MAX_RECOVERY_TIME, &config->WINNING_THRESHOLD, &config->NUM_PLAYERS);
}

