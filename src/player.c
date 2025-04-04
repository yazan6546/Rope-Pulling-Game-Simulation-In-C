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
#include <sys/mman.h>
#include "game.h"

int write_fd;
Team my_team;
Player *current_player;
float previous_energy = 0;


volatile sig_atomic_t energy_update = 0;
volatile sig_atomic_t recovery_complete = 0;
Game *game;
volatile sig_atomic_t is_round_reset = 0;
volatile sig_atomic_t remaining_recovery_time = 0;

void handle_alarm(int signum) {
    if (current_player->state == FALLEN || current_player->state == EXHAUSTED) {
        remaining_recovery_time--;
        // recovery_complete = 1;
    }

    energy_update = 1;
    alarm(1);  // Schedule next energy update
}

void process_player_state() {
    if(energy_update) {
        if (current_player->state == PULLING) {

        current_player->attributes.energy -= (current_player->attributes.rate_decay * (float) current_player->position);


        // check for random falls
        if (random_float(0, 1) < current_player->attributes.falling_chance) {
            previous_energy = current_player->attributes.energy;
            current_player->attributes.energy = 0;
            print_with_time1(game, "Player %d (Team %d) has fallen! (Energy = %f)\n", current_player->number, my_team, previous_energy);
            current_player->state = FALLEN;
            remaining_recovery_time = current_player->attributes.recovery_time;  // Set recovery timer
            fflush(stdout);
        } else if (current_player->attributes.energy <= 0) {
            current_player->attributes.energy = 0;
            print_with_time1(game, "Player %d (Team %d) is exhausted!\n",
            current_player->number, my_team);
            current_player->state = EXHAUSTED;
            remaining_recovery_time = current_player->attributes.recovery_time;  // Set recovery timer
            fflush(stdout);
        }

            energy_update = 0; // this way, we ensure that energy is only updated once the alarm handler is called
        }

        else if (current_player->state == FALLEN && remaining_recovery_time == 0) {
            current_player->attributes.energy = previous_energy;
            current_player->state = PULLING;
            print_with_time1(game, "Player %d (Team %d) rejoining with energy %.2f\n",
                current_player->number, my_team, current_player->attributes.energy);
            fflush(stdout);
            // alarm(1);  // Restart energy updates
        }
        else if (current_player->state == EXHAUSTED && remaining_recovery_time == 0) {
            current_player->attributes.energy = current_player->attributes.inital_energy *
                                                current_player->attributes.endurance;
            current_player->state = PULLING;
            print_with_time1(game, "Player %d (Team %d) recovering with energy %.2f\n",
                current_player->number, my_team, current_player->attributes.energy);
            fflush(stdout);
        }

    // send energy updates every 1 sec
    // if (energy_update){
    float effort = current_player->attributes.energy * ((float) current_player->position);
    write(write_fd, &effort, sizeof(float));
    // } else {
    //     alarm(1);  // Schedule next energy update
    // }


    }

}

void handle_get_ready(int signum) {
    print_with_time1(game, "Player %d (Team %d) getting ready\n", current_player->number, my_team);

    if (current_player->position != current_player->new_position) {
        print_with_time1(game, "Player %d (Team %d) Repositioned from %d to %d\n",
        current_player->number, my_team, current_player->position, current_player->new_position);

        current_player->position = current_player->new_position;
    }
    current_player->state = READY;
    fflush(stdout);
}

void handle_start(int signum) {
    print_with_time1(game, "Player %d (Team %d) starting to pull %s\n",
           current_player->number,
           my_team,
           my_team == TEAM_A ? "right" : "left");
    current_player->state = PULLING;
    fflush(stdout);

    alarm(1);  // Start energy updates
}

void reset_round(int signum) {
    is_round_reset = 1;
    energy_update = 0;
}

int main(int argc, char *argv[]) {
    // printf("argv[1] = %s\n", argv[1]);


    // Updated argument check: <serialized_data> <energy_write_fd> <pos_pipe_fd> <fd>
    if (argc < 5) {
        fprintf(stderr, "Usage: player <serialized_data> <energy_write_fd> <pos_pipe_fd> <fd>\n");
        exit(1);
    }

    int fd = atoi(argv[4]);  // 4th argument for mmap

    // Open the file descriptor for reading
    game = mmap(NULL, sizeof(Game), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    fflush(stdout);

    init_random(getpid());

    current_player = create_player(getpid());
    deserialize_player(current_player, argv[1]);
    print_player(current_player);

    current_player->attributes.inital_energy = current_player->attributes.energy;

    my_team = current_player->team;
    write_fd = atoi(argv[2]);
    int pos_pipe_fd = atoi(argv[3]);  // Dedicated new position pipe

    // Set up signal handlers
    signal(SIGALRM, handle_alarm);
    signal(SIGUSR1, handle_get_ready);
    signal(SIGUSR2, handle_start);
    signal(SIGHUP, reset_round);


    while(1) {
        // wait for a signal
        pause();
        // reset round
        if(is_round_reset) {
            // Read new position from the dedicated position pipe
            alarm(0);  // Cancel the alarm to avoid multiple signals
            int new_position;

            current_player->attributes.energy = current_player->attributes.inital_energy *
                                                current_player->attributes.endurance;

            if (write(write_fd, &current_player->attributes.energy, sizeof(float)) <= 0) {
                perror("write");
            }
            if (read(pos_pipe_fd, &new_position, sizeof(int)) > 0) {
                current_player->new_position = new_position;
            }
            else {
                perror("read");
            }

            fflush(stdout);
            current_player->state = IDLE;
            is_round_reset = 0;

            print_with_time1(game, "Player %d (Team %d) resetting round with energy : %f\n", current_player->number, my_team, current_player->attributes.energy);

            pause();
        }
        // continue simulation
        process_player_state();
    }

    return 0;
}

Player *create_player(pid_t pid) {
    Player *player = (Player *) malloc(sizeof(Player));
    if (player == NULL) {
        perror("Failed to allocate memory for player");
        exit(EXIT_FAILURE);
    }

    player->pid = pid;

    return player;

}