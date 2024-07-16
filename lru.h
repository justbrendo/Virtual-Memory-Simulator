#pragma once

#include "main.h"
#include <stdio.h>
#include <stdlib.h>

void lru_algorithm(unsigned address, unsigned num_frames, unsigned *used_frames);
void lru_init(arguments *args);