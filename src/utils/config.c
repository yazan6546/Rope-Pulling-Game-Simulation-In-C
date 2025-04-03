#include "config.h"

// Function to load configuration settings from a specified file
int load_config(const char *filename, Config *config) {
    // Attempt to open the config file in read mode
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening config file"); // Print an error message if file opening fails
        return -1; // Return error code
    }

    // Initialize all configuration values to default or invalid values to indicate uninitialized state
    config->MIN_ENERGY = -1;
    config->MAX_ENERGY = -1;
    config->MAX_SCORE = -1;
    config->MAX_TIME = -1;
    config->MAX_ROUND_TIME = -1;
    config->NUM_ROUNDS = -1;
    config->MIN_RATE_DECAY = -1;
    config->MAX_RATE_DECAY = -1;
    config->MIN_RECOVERY_TIME = -1;
    config->MAX_RECOVERY_TIME = -1;
    config->WINNING_THRESHOLD = -1;
    config->NUM_PLAYERS = -1;
    config->UPDATE_RATE = -1;
    config->MIN_FALLING_CHANCE = -1;
    config->MAX_FALLING_CHANCE = -1;
    config->MIN_ENDURANCE = -1;
    config->MAX_ENDURANCE = -1;

    // Buffer to hold each line from the configuration file
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Ignore comments and empty lines
        if (line[0] == '#' || line[0] == '\n') continue;

        // Parse each line as a key-value pair
        char key[50];
        float value;
        if (sscanf(line, "%40[^=]=%f", key, &value) == 2) {

            // Set corresponding config fields based on the key
            if (strcmp(key, "MIN_ENERGY") == 0) config->MIN_ENERGY = value;
            else if (strcmp(key, "MAX_ENERGY") == 0) config->MAX_ENERGY = value;
            else if (strcmp(key, "MAX_SCORE") == 0) config->MAX_SCORE = value;
            else if (strcmp(key, "MAX_TIME") == 0) config->MAX_TIME = value;
            else if (strcmp(key, "NUM_ROUNDS") == 0) config->NUM_ROUNDS = value;
            else if (strcmp(key, "MIN_RATE_DECAY") == 0) config->MIN_RATE_DECAY = value;
            else if (strcmp(key, "MAX_RATE_DECAY") == 0) config->MAX_RATE_DECAY = value;
            else if (strcmp(key, "MIN_RECOVERY_TIME") == 0) config->MIN_RECOVERY_TIME = value;
            else if (strcmp(key, "MAX_RECOVERY_TIME") == 0) config->MAX_RECOVERY_TIME = value;
            else if (strcmp(key, "WINNING_THRESHOLD") == 0) config->WINNING_THRESHOLD = value;
            else if (strcmp(key, "NUM_PLAYERS") == 0) config->NUM_PLAYERS = value;
            else if (strcmp(key, "UPDATE_RATE") == 0) config->UPDATE_RATE = value;
            else if (strcmp(key, "MIN_FALLING_CHANCE") == 0) config->MIN_FALLING_CHANCE = value;
            else if (strcmp(key, "MAX_FALLING_CHANCE") == 0) config->MAX_FALLING_CHANCE = value;
            else if (strcmp(key, "MAX_ROUND_TIME") == 0) config->MAX_ROUND_TIME = value;
            else if (strcmp(key, "MIN_ENDURANCE") == 0) config->MIN_ENDURANCE = value;
            else if (strcmp(key, "MAX_ENDURANCE") == 0) config->MAX_ENDURANCE = value;
            else {
                fprintf(stderr, "Unknown key: %s\n", key); // Print error for unknown keys
                fclose(file); // Close the config file
                return -1; // Return error code
            }
        }
    }

    fclose(file); // Close the config file

    // Debug print if __CLI is defined
#ifdef __DEBUG
    printf("Config values: \n MIN_ENERGY: %d\n"
           "MAX_ENERGY: %d\n MAX_SCORE: %d\n"
           "MAX_TIME: %d\n NUM_ROUNDS: %d\n"
           "MIN_RATE_DECAY: %d\n MAX_RATE_DECAY: %d\n"
           , config->MIN_ENERGY,
           config->MAX_ENERGY,
           config->MAX_SCORE,
           config->MAX_TIME,
           config->NUM_ROUNDS,
           config->MIN_RATE_DECAY,
           config->MAX_RATE_DECAY);


fflush(stdout);
#endif


    // Check if all required parameters are set
    return check_parameter_correctness(config);

}


void print_config(Config *config) {
    printf("Config values: \n MIN_ENERGY: %f\n"
           "MAX_ENERGY: %f\n MAX_SCORE: %f\n"
           "MAX_TIME: %f\n NUM_ROUNDS: %f\n"
           "MIN_RATE_DECAY: %f\n MAX_RATE_DECAY: %f\n"
           "MIN RECOVERY TIME: %f\n MAX RECOVERY TIME: %f\n"
           "WINNING THRESHOLD: %f\n"
           "NUM_PLAYERS: %d\n"
           "UPDATE_RATE: %f\n"
           "MIN_FALLING_CHANCE: %f\n"
           "MAX_FALLING_CHANCE: %f\n"
           "MAX_ROUND_TIME: %d\n"
           "MIN_ENDURANCE: %f\n"
           "MAX_ENDURANCE: %f\n",
           config->MIN_ENERGY,
           config->MAX_ENERGY,
           config->MAX_SCORE,
           config->MAX_TIME,
           config->NUM_ROUNDS,
           config->MIN_RATE_DECAY,
           config->MAX_RATE_DECAY,
           config->MIN_RECOVERY_TIME,
           config->MAX_RECOVERY_TIME,
           config->WINNING_THRESHOLD,
           config->NUM_PLAYERS,
           config->UPDATE_RATE,
           config->MIN_FALLING_CHANCE,
           config->MAX_FALLING_CHANCE,
           config->MAX_ROUND_TIME,
           config->MIN_ENDURANCE,
           config->MAX_ENDURANCE);

    fflush(stdout);
}

int check_parameter_correctness(const Config *config) {

    if (config->MIN_ENERGY < 0 || config->MAX_ENERGY < 0 || config->MIN_RATE_DECAY < 0 || config->MAX_RATE_DECAY < 0 ||
     config->MIN_RECOVERY_TIME < 0 || config->MAX_RECOVERY_TIME < 0 || config->MIN_FALLING_CHANCE < 0 ||
     config->MAX_FALLING_CHANCE < 0 || config->MIN_ENDURANCE < 0 || config->MAX_ENDURANCE < 0 || config->NUM_ROUNDS < 0 ||
     config->NUM_PLAYERS < 0 || config->UPDATE_RATE < 0 || config->MAX_ROUND_TIME < 0 || config->WINNING_THRESHOLD < 0 ||
     config->MAX_SCORE < 0 || config->MAX_TIME < 0) {

        fprintf(stderr, "Values must be greater than or equal to 0\n"); // Print error for invalid energy
        return -1; // Return error if invalid energy
     }


    if (config->MIN_ENERGY > config->MAX_ENERGY) {
        fprintf(stderr, "MIN_ENERGY cannot be greater than MAX_ENERGY\n"); // Print error for invalid range
        return -1; // Return error if range is invalid
    }

    if (config->MIN_RATE_DECAY > config->MAX_RATE_DECAY) {
        fprintf(stderr, "MIN_RATE_DECAY cannot be greater than MAX_RATE_DECAY\n"); // Print error for invalid range
        return -1; // Return error if range is invalid
    }

    if (config->MIN_RECOVERY_TIME > config->MAX_RECOVERY_TIME) {
        fprintf(stderr, "MIN_RECOVERY_TIME cannot be greater than MAX_RECOVERY_TIME\n"); // Print error for invalid range
        return -1; // Return error if range is invalid
    }

    if (config->MIN_FALLING_CHANCE > config->MAX_FALLING_CHANCE) {
        fprintf(stderr, "MIN_FALLING_CHANCE cannot be greater than MAX_FALLING_CHANCE\n"); // Print error for invalid range
        return -1; // Return error if range is invalid
    }

    if (config->MIN_ENDURANCE > config->MAX_ENDURANCE) {
        fprintf(stderr, "MIN_ENDURANCE cannot be greater than MAX_ENDURANCE\n"); // Print error for invalid range
        return -1; // Return error if range is invalid
    }

    if (config->NUM_PLAYERS <= 0) {
        fprintf(stderr, "NUM_PLAYERS must be greater than 0\n"); // Print error for invalid number of players
        return -1; // Return error if invalid number of players
    }
    if (config->NUM_PLAYERS % 2 != 0) {
        fprintf(stderr, "NUM_PLAYERS must be even\n"); // Print error for invalid number of players
        return -1; // Return error if invalid number of players
    }

    if (config->MIN_RECOVERY_TIME > config->MAX_RECOVERY_TIME) {
        fprintf(stderr, "MIN_RECOVERY_TIME cannot be greater than MAX_RECOVERY_TIME\n"); // Print error for invalid recovery time
        return -1; // Return error if invalid recovery time
    }

    if (config->MIN_FALLING_CHANCE > config->MAX_FALLING_CHANCE) {
        fprintf(stderr, "MIN_FALLING_CHANCE cannot be greater than MAX_FALLING_CHANCE\n"); // Print error for invalid falling chance
        return -1; // Return error if invalid falling chance
    }

    if (config->MIN_ENDURANCE > config->MAX_ENDURANCE) {
        fprintf(stderr, "MIN_ENDURANCE cannot be greater than MAX_ENDURANCE\n"); // Print error for invalid endurance
        return -1; // Return error if invalid endurance
    }

    if (config->UPDATE_RATE <= 0) {
        fprintf(stderr, "UPDATE_RATE must be greater than 0\n"); // Print error for invalid update rate
        return -1; // Return error if invalid update rate
    }

    if (config->MIN_ENDURANCE > 1 || config->MAX_ENDURANCE > 1) {
        fprintf(stderr, "MIN_ENDURANCE and MAX_ENDURANCE must be between 0 and 1\n"); // Print error for invalid endurance
        return -1; // Return error if invalid endurance
    }

    return 0; // Return success if all checks pass
}

