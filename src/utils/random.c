//
// Created by - on 3/23/2025.
//

#include "random.h"

float random_float(float min, float max) {
    unsigned int seed = (unsigned int)time(NULL);
    srand(seed);
    return rand() / (float)RAND_MAX * (max - min) + min;
}
