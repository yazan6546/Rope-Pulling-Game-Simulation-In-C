//
// Created by - on 3/23/2025.
//

#include "player.h"
#include "player_utils.h"

int main(int argc, char *argv[]) {


    printf("Player process\n");
    printf("argv[1] = %s\n", argv[1]);
    Player player;
    deserialize_player(&player, argv[1]);

    print_player(&player);
    fflush(stdout);

    return 0;
}


void print_player(Player *player) {
    printf("Player: \n"
           "rate_decay: %f\n"
           "energy: %f\n"
           "recovery_time: %f\n"
           "team: %d\n"
           "number: %d\n", player->rate_decay, player->energy, player->recovery_time, player->team, player->number);
}
