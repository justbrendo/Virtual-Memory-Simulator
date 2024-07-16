#pragma once

// Typedef for function pointers
typedef void (*algorithm_func_t)();

// Struct for algorithm name and function mapping
typedef struct {
    const char *name;
    algorithm_func_t func;
    algorithm_func_t init;
} algorithm_mapping_t;

// Extern declaration for the algorithms array
extern algorithm_mapping_t algorithms[];