//
// Created by - on 3/24/2025.
//

#include "player_utils.h"
#include "random.h"

void deserialize_player(Player *player, char *buffer) {
    sscanf(buffer, "%f %f %f %d %d", &player->rate_decay, &player->energy, &player->recovery_time,
            &player->team, &player->number);
}

void serialize_player(Player *player, char *buffer) {
    sprintf(buffer, "%f %f %f %d %d", player->rate_decay, player->energy, player->recovery_time,
            player->team, player->number);
}

