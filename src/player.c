//
// Created by - on 3/23/2025.
//

#include "player.h"

int main(int argc, char *argv[]) {

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
