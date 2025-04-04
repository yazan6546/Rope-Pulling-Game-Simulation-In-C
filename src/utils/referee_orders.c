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

void align(Player* team, int num_players) {
    // Use qsort to sort team by energy level
    qsort(team, num_players, sizeof(Player), compare_players);
    
    for (int i = 0; i < num_players; i++) {
        team[i].new_position = i+1;

        if (team[i].new_position != team[i].position) {
            printf("From referee : Player %d (Team %d) Repositioned from %d to %d\n\n",
                            team[i].number, team[i].team, team[i].position, team[i].new_position);
        }
    }
}