//
// Created by - on 3/24/2025.
//

#ifndef PLAYER_UTILS_H
#define PLAYER_UTILS_H


#include "player.h"

void deserialize_player(Player *player, char *buffer);
void serialize_player(Player *player, char *buffer);

#endif //PLAYER_UTILS_H
