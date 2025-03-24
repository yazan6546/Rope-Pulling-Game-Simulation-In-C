//
// Created by - on 3/23/2025.
//

#include "player.h"
#include <signal.h>
#include "player_utils.h"

#define FALL_CHANCE 0.05  // 5% chance to fall each second

Team my_team;
Player *current_player;

void handle_get_ready(int signum) {
    printf("Player %d (Team %d) getting ready\n", current_player->number, my_team);
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
        if (random_float(0, 1) < FALL_CHANCE) {
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
        sleep(current_player->recovery_time);
        current_player->energy = random_float(config.MIN_ENERGY/2, config.MAX_ENERGY/2);  // Recover some energy
        current_player->state = PULLING;
        printf("Player %d (Team %d) rejoining with energy %.2f\n", 
               current_player->number, my_team, current_player->energy);
        fflush(stdout);
    }
}

int main(int argc, char *argv[]) {
    printf("Player process\n");
    printf("argv[1] = %s\n", argv[1]);

    // Parse config and team
    deserialize_player(current_player, argv[1]);
    my_team = current_player->team;
    fflush(stdout);

    // Setup signal handlers
    signal(SIGUSR1, handle_get_ready);
    signal(SIGUSR2, handle_start);
    
    // Wait for signals
    while(1) {
        if (current_player->state == PULLING || current_player->state == RECOVERING) {
            simulate_pulling();
            // send data to referee
            usleep(100000);  // Small delay to control simulation speed (or use alarm?)
        }
    }
    
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
