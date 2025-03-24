//
// Created by - on 3/23/2025.
//

#include "player.h"
#include "player_utils.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

int write_fd;
Player player;
int position;

void send_energy(int signum) {
    // Decrease energy based on rate_decay * position
    player.energy -= player.rate_decay * position;
    if (player.energy < 0.0f) {
        player.energy = 0.0f;
    }

    // Send energy to parent
    write(write_fd, &player.energy, sizeof(float));

    // Schedule next alarm
    alarm(1);
}

int main(int argc, char *argv[]) {

    printf("Player process\n");

    if (argc < 3) {
        fprintf(stderr, "Usage: player <serialized_data> <write_fd>\n");
        exit(1);
    }

    printf("argv[1] = %s\n", argv[1]);
    deserialize_player(&player, argv[1]);

    print_player(&player);

    write_fd = atoi(argv[2]);
    position = player.number + 1;  // Position from 1 to 4

    // Close the read end (not used)
    // Not needed explicitly as it's not opened in player

    // Set up signal handler
    signal(SIGALRM, send_energy);
    alarm(1); // Trigger first after 1 second

    while (1) {
        pause();  // Wait for signal
    }

    close(write_fd);
    return 0;
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
