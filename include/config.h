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
    int MAX_ROUND_TIME;
    float NUM_ROUNDS;
    float MIN_RECOVERY_TIME;
    float MAX_RECOVERY_TIME;
    float WINNING_THRESHOLD;
    int NUM_PLAYERS;
    float UPDATE_RATE;
    float MIN_FALLING_CHANCE;
    float MAX_FALLING_CHANCE;
    float MIN_ENDURANCE;
    float MAX_ENDURANCE;

} Config;

int load_config(const char *filename, Config *config);
void print_config(Config *config);
int check_parameter_correctness(const Config *config);
#endif // CONFIG_H
