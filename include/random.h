//
// Created by - on 3/23/2025.
//

#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>
#include <time.h>
#include "config.h"

float random_float(float min, float max);
void generate_all_random_values(Config *config, float *values);

#endif //RANDOM_H
