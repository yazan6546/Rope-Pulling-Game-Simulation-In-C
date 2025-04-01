//
// Created by - on 3/23/2025.
//

#include "player.h"
#include <signal.h>
#include "player_utils.h"
#include <stdlib.h>
#include <unistd.h>
#include "random.h"
#include <sys/types.h>
#include <stdarg.h>

int pipe_fds[2];
Team my_team;
Player *current_player;
unsigned int previous_energy = 0;
unsigned int remaining_recovery_time = 0;

volatile sig_atomic_t energy_update = 0;
volatile sig_atomic_t recovery_complete = 0;
volatile sig_atomic_t elapsed_time = 0;

void handle_alarm(int signum) {
    if (current_player->state == RECOVERING) {
        remaining_recovery_time--;
        // recovery_complete = 1;
    }

    elapsed_time++;
    energy_update = 1;
    alarm(1);  // Schedule next energy update
}

void process_player_state() {
    if(energy_update) {
        if (current_player->state == PULLING) {

            current_player->energy -= current_player->rate_decay;

            // check for random falls
            if (random_float(0, 1) < current_player->falling_chance) {
                previous_energy = current_player->energy;
                current_player->energy = 0;
                print_with_time("Player %d (Team %d) has fallen!\n", current_player->number, my_team);
                current_player->state = RECOVERING;
                remaining_recovery_time = current_player->recovery_time;  // Set recovery timer
                fflush(stdout);
            } else if (current_player->energy <= 0) {
                current_player->energy = 0;
                print_with_time("Player %d (Team %d) is exhausted!\n",
                current_player->number, my_team);
                current_player->state = RECOVERING;
                remaining_recovery_time = current_player->recovery_time;  // Set recovery timer
                fflush(stdout);
            }

            energy_update = 0; // this way, we ensure that energy is only updated once the alarm handler is called
        }

        else if (current_player->state == RECOVERING && remaining_recovery_time == 0) {
            current_player->energy = previous_energy;
            current_player->state = PULLING;
            print_with_time("Player %d (Team %d) rejoining with energy %.2f\n",
                current_player->number, my_team, current_player->energy);
            fflush(stdout);
            // alarm(1);  // Restart energy updates
        }

        // send energy updates every 1 sec
        float effort = current_player->energy * ((float) current_player->position);
        write(write_fd, &effort, sizeof(float));
        fflush(stdout);
    }

}

void handle_get_ready(int signum) {
    print_with_time("Player %d (Team %d) getting ready\n", current_player->number, my_team);
    print_with_time("Player %d (Team %d) Repositioned from %d to %d\n", current_player->number, my_team, current_player->position, current_player->new_position);
    current_player->position = current_player->new_position;
    current_player->state = READY;
    fflush(stdout);
}

void handle_start(int signum) {
    print_with_time("Player %d (Team %d) starting to pull %s\n",
           current_player->number,
           my_team,
           my_team == TEAM_A ? "right" : "left");
    current_player->state = PULLING;
    fflush(stdout);

    alarm(1);  // Start energy updates
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
    pipe_fds[1] = atoi(argv[2]);
    pipe_fds[0] = atoi(argv[3]);


    // Close the read end (not used)
    // Not needed explicitly as it's not opened in player

    // Set up signal handlers
    signal(SIGALRM, handle_alarm);
    signal(SIGUSR1, handle_get_ready);
    signal(SIGUSR2, handle_start);

    while(1) {
        pause();
        process_player_state();
    }

    return 0;
}

Player *create_player(pid_t pid) {
    Player *player = (Player *) malloc(sizeof(Player));
    player->pid = pid;

    return player;

}

void print_with_time(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("@ %ds: ", elapsed_time);
    vprintf(format, args);
    va_end(args);
}