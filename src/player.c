//
// Created by - on 3/23/2025.
//

#include "player.h"
#include "player_utils.h"
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    printf("Player process\n");
    printf("argv[1] = %s\n", argv[1]);
    Player player;
    deserialize_player(&player, argv[1]);

    print_player(&player);
    int write_fd = 0;
    if (argc >= 3) {
        write_fd = atoi(argv[2]);
        write(write_fd, &player.energy, sizeof(float));
        close(write_fd);
    }

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
