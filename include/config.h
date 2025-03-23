// include/config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int MIN_RATE_DECAY;
    int MAX_RATE_DECAY;
    int MIN_ENERGY;
    int MAX_ENERGY;
    int MAX_SCORE;
    int MAX_TIME;
    int NUM_ROUNDS
} Config;

int load_config(const char *filename, Config *config);
void print_config(Config *config);

#endif // CONFIG_H
