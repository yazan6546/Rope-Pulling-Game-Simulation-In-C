#include "referee_orders.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    int temp1[num_players];
    int temp2[num_players];
    
    for (int i = 0; i < num_players; i++) {
        team[i].new_position = i+1;
    }

    for (int i = 0; i < num_players; i++) {
        int index = team[i].position - 1;
        temp1[index] = read_fds[i];
        temp2[index] = pos_pipe_fds[i];
    }

    memcpy(read_fds, temp1, sizeof(int) * num_players);
    memcpy(pos_pipe_fds, temp2, sizeof(int) * num_players);
}