#ifndef REFEREE_ORDERS_H
#define REFEREE_ORDERS_H

#include "player.h"

void get_ready(int signum);
void start(int signum);

void align(Player* team, int num_players);

#endif // REFEREE_ORDERS_H