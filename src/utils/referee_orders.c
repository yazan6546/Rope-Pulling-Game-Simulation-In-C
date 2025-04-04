#include "referee_orders.h"
#include <stdio.h>
#include <stdlib.h>

// Add this comparison function outside align
static int compare_players(const void* a, const void* b) {
    Player* p1 = (Player*)a;
    Player* p2 = (Player*)b;
    if (p1->attributes.energy > p2->attributes.energy) return 1;
    if (p1->attributes.energy < p2->attributes.energy) return -1;
    return 0;
}

void align(Player* team, int num_players, int *read_fds, int *pos_pipe_fds) {
    // Use qsort to sort team by energy level
    qsort(team, num_players, sizeof(Player), compare_players);
    
    for (int i = 0; i < num_players; i++) {
        team[i].new_position = i+1;
        // Print the new position for debugging
        printf("Referee DEBUG : Player %d old position : %d new position: %d\n", team[i].number, team[i].position ,team[i].new_position);
    }

    for (int i = 0; i < num_players; i++) {
        const int new_index = team[i].new_position - 1;
        const int old_index = team[i].position - 1;

        int temp = pos_pipe_fds[old_index];
        pos_pipe_fds[old_index] = pos_pipe_fds[new_index];
        pos_pipe_fds[new_index] = temp;

        temp = read_fds[old_index];
        read_fds[old_index] = read_fds[new_index];
        read_fds[new_index] = temp;

    }
}