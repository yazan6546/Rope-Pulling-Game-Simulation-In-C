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
    char buffer[1000];
    int length = sprintf(buffer, "Player: %d\n"
           "rate_decay: %f\n"
           "energy: %f\n"
           "recovery_time: %f\n"
           "team: %d\n"
           "falling_chance: %f\n"
           "old position: %d\n"
           "new position: %d\n"
           "endurance: %f\n\n",
           player->number,
           player->attributes.rate_decay,
           player->attributes.energy,
           player->attributes.recovery_time,
           player->team,
           player->attributes.falling_chance,
           player->position,
           player->new_position,
           player->attributes.endurance);

    // Use a single atomic write operation
    write(STDOUT_FILENO, buffer, length);
    write(STDOUT_FILENO, "\n", 1);
}