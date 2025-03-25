//
// Created by - on 3/23/2025.
//

#include "player.h"
#include <signal.h>
#include "player_utils.h"
#include <stdlib.h>
#include <unistd.h>
#include "random.h"

int write_fd;
Team my_team;
Player *current_player;

void send_energy(int signum) {
    // Decrease energy based on rate_decay * position
    const int position = current_player->number + 1;
    current_player->energy -= current_player->rate_decay * position;
    if (current_player->energy < 0.0f) {
        printf("ok\n");
        current_player->energy = 0.0f;
    }

    // Send energy to parent
    write(write_fd, &current_player->energy, sizeof(float));

    // Schedule next alarm
    alarm(1);
}

void handle_get_ready(int signum) {
    printf("Player %d (Team %d) getting ready\n", current_player->number, my_team);
    printf("Player %d (Team %d) Repositioned from %d to %d\n", current_player->number, my_team, current_player->position, current_player->new_position);
    current_player->position = current_player->new_position;
    current_player->state = READY;
    fflush(stdout);
}

void handle_start(int signum) {
    printf("Player %d (Team %d) starting to pull %s\n",
           current_player->number,
           my_team,
           my_team == TEAM_A ? "right" : "left");
    current_player->state = PULLING;
    fflush(stdout);
}

void simulate_pulling() {
    if (current_player->state == PULLING) {
        // Decrease energy based on rate_decay
        current_player->energy -= current_player->rate_decay;

        // Check if player falls (random chance)
        if (random_float(0, 1) < current_player->falling_chance) {
            printf("Player %d (Team %d) has fallen!\n", current_player->number, my_team);
            current_player->state = RECOVERING;
            fflush(stdout);
        } else if (current_player->energy <= 0) { // Check if energy is depleted
            current_player->energy = 0;
            current_player->state = RECOVERING;
            printf("Player %d (Team %d) is exhausted!\n", current_player->number, my_team);
            fflush(stdout);
        }
    }
    else if (current_player->state == RECOVERING) {
        printf("Player %d (Team %d) recovering\n", current_player->number, my_team);
        fflush(stdout);
        sleep((unsigned int) current_player->recovery_time);
        current_player->state = PULLING;
        printf("Player %d (Team %d) rejoining with energy %.2f\n",
               current_player->number, my_team, current_player->energy);
        fflush(stdout);
    }
}

int main(int argc, char *argv[]) {
    // printf("argv[1] = %s\n", argv[1]);


    if (argc < 3) {
        fprintf(stderr, "Usage: player <serialized_data> <write_fd>\n");
        exit(1);
    }

    current_player = create_player(getpid());
    deserialize_player(current_player, argv[1]);

    my_team = current_player->team;
    write_fd = atoi(argv[2]);

    // Close the read end (not used)
    // Not needed explicitly as it's not opened in player

    // Set up signal handler
    signal(SIGALRM, send_energy);
    signal(SIGUSR1, handle_get_ready);
    signal(SIGUSR2, handle_start);

    alarm(1); // Trigger first after 1 second

    while (1) {
        pause();  // Wait for signal
    }
    // close(write_fd);
    // return 0;
    // // Parse config and team
    // deserialize_player(current_player, argv[1]);
    // my_team = current_player->team;
    // int write_fd = 0;
    // fflush(stdout);


    // // Wait for signals
    // while(1) {
    //     if (current_player->state == PULLING || current_player->state == RECOVERING) {
    //         simulate_pulling();
    //         // send data to referee
    //
    //         if (argc >= 3) {
    //             write_fd = atoi(argv[2]);
    //             write(write_fd, &current_player->energy, sizeof(float));
    //             close(write_fd);
    //         }
    //
    //
    //         usleep(100000);  // Small delay to control simulation speed (or use alarm?)
    //     }
    // }

}


Player *create_player(pid_t pid) {
    Player *player = (Player *) malloc(sizeof(Player));
    player->pid = pid;

    return player;

}