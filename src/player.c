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
    //
    // printf("in player....");
    // print_player(&player);


    fflush(stdout);

    return 0;
}
