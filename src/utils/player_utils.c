//
// Created by - on 3/24/2025.
//

#include "player_utils.h"
#include "random.h"

void deserialize_player(Player *player, char *buffer) {
    sscanf(buffer, "%f %f %f %d %d %f %d %d %f", &player->attributes.rate_decay, &player->attributes.energy, &player->attributes.recovery_time,
            &player->team, &player->number, &player->attributes.falling_chance, &player->position, &player->new_position, &player->attributes.endurance);
}

void serialize_player(Player *player, char *buffer) {
    sprintf(buffer, "%f %f %f %d %d %f %d %d %f", player->attributes.rate_decay, player->attributes.energy, player->attributes.recovery_time,
            player->team, player->number, player->attributes.falling_chance, player->position, player->new_position, player->attributes.endurance);
}

void print_player(Player *player) {
    printf("Player: \n"
           "rate_decay: %f\n"
           "energy: %f\n"
           "recovery_time: %f\n"
           "team: %d\n"
           "number: %d\n"
           "falling_chance: %f\n"
           "old position: %d\n"
           "new position: %d\n"
           "endurance: %f\n",
           player->attributes.rate_decay,
           player->attributes.energy,
           player->attributes.recovery_time,
           player->team,
           player->number,
           player->attributes.falling_chance,
           player->position,
           player->new_position,
           player->attributes.endurance);

    fflush(stdout);
}
