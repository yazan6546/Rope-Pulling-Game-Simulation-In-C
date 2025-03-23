// include/config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    float MIN_RATE_DECAY;
    float MAX_RATE_DECAY;
    float MIN_ENERGY;
    float MAX_ENERGY;
    float MAX_SCORE;
    float MAX_TIME;
    float NUM_ROUNDS;
    float MIN_RECOVERY_TIME;
    float MAX_RECOVERY_TIME;
    float WINNING_THRESHOLD;
    int NUM_PLAYERS;
} Config;

int load_config(const char *filename, Config *config);
void print_config(Config *config);
void config_to_string(const Config *config, char *buffer);
#endif // CONFIG_H
