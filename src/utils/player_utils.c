//
// Created by - on 3/24/2025.
//

#include "player_utils.h"
#include "random.h"

void deserialize_player(Player *player, char *buffer) {
    sscanf(buffer, "%f %f %f %d %d %f", &player->rate_decay, &player->energy, &player->recovery_time,
            &player->team, &player->number, &player->falling_chance);
}

void serialize_player(Player *player, char *buffer) {
    sprintf(buffer, "%f %f %f %d %d %f", player->rate_decay, player->energy, player->recovery_time,
            player->team, player->number, player->falling_chance);
}

void print_player(Player *player) {
    printf("Player: \n"
           "rate_decay: %f\n"
           "energy: %f\n"
           "recovery_time: %f\n"
           "team: %d\n"
           "number: %d\n",
           player->rate_decay,
           player->energy,
           player->recovery_time,
           player->team,
           player->number);
}
