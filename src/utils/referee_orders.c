#include "referee_orders.h"
#include <stdio.h>
#include <stdlib.h>

// Add this comparison function outside align
static int compare_players(const void* a, const void* b) {
    Player* p1 = (Player*)a;
    Player* p2 = (Player*)b;
    if (p1->energy > p2->energy) return 1;
    if (p1->energy < p2->energy) return -1;
    return 0;
}

void align(Player* team, int num_players) {
    // Use qsort to sort team by energy level
    qsort(team, num_players, sizeof(Player), compare_players);
    
    printf("Align\n");
    for (int i = 0; i < num_players; i++) {
        printf("Player %d: energy = %.2f\n", team[i].number, team[i].energy);
    }
}