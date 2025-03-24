#include "common.h"
#include "config.h"
#include "random.h"
#include "player.h"
#include <signal.h>
#include "referee_orders.h"

Config config;
pid_t *team_a_pids;
pid_t *team_b_pids;

int main() {

    int id = fork();

    if (id == 0) {
        printf("Child process\n");
        execl("/bin/pwd", "pwd", NULL);
    } else {
        wait(NULL);
        printf("Parent process\n");
    }
    load_config("../config.txt", &config);

    print_config(&config);

    // Allocate memory for player PIDs
    team_a_pids = malloc(sizeof(pid_t) * config.NUM_PLAYERS/2);
    team_b_pids = malloc(sizeof(pid_t) * config.NUM_PLAYERS/2);

    fork_players(TEAM_A, config.NUM_PLAYERS/2);
    sleep(1); // Give time for processes to start

    fork_players(TEAM_B, config.NUM_PLAYERS/2);
    sleep(1);

    // Send get ready signal to all players
    for (int i = 0; i < config.NUM_PLAYERS/2; i++) {
        kill(team_a_pids[i], SIGUSR1);
        kill(team_b_pids[i], SIGUSR1);
    }
    
    // the referee aligns the players, why send a signal to the players to align themselves?
    align(team_a_pids, config.NUM_PLAYERS/2);
    align(team_b_pids, config.NUM_PLAYERS/2);

    sleep(2); // Wait for players to get ready

    // Send start signal to all players
    for (int i = 0; i < config.NUM_PLAYERS/2; i++) {
        kill(team_a_pids[i], SIGUSR2);
        kill(team_b_pids[i], SIGUSR2);
    }

    // Clean up
    free(team_a_pids);
    free(team_b_pids);

    return 0;
}

void fork_players(Team team, int num_players) {
    for (int i = 0; i < num_players; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
        }

        else if (pid == 0) {
            char buffer[100];
            config_to_string(&config, buffer);
            char team_str[2];
            // Convert team to string to send through execl
            sprintf(team_str, "%d", team);
            // Execl sends buffer and team_str as arguments to player
            if (execl("./player", "player", buffer, team_str, NULL)) {
                perror("execl");
            }
        }
        else {
            // Store PID in appropriate array
            if (team == TEAM_A) {
                team_a_pids[i] = pid;
            } else {
                team_b_pids[i] = pid;
            }
            printf("Parent process spawned player with PID: %d\n", pid);
            fflush(stdout);
        }
    }

}



