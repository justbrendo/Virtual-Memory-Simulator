#include "algorithms.h"

#include <stdio.h>

#include "2a.h"
#include "fifo.h"
#include "lru.h"
#include "main.h"
#include "random.h"
#include "second_chance.h"

// Definition of the algorithms array
algorithm_mapping_t algorithms[] = {
    {"lru", lru_algorithm, lru_init},
    {"2a", second_chance_algorithm, second_chance_init},
    {"fifo", fifo_algorithm, fifo_init},
    {"random", random_algorithm, random_init},
    {NULL, NULL, NULL}  // Sentinel value to mark the end of the array
};