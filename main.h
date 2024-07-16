#pragma once

void usage(const char *program_name);

typedef void (*algorithm_func_t)();

typedef struct {
    const char *algorithm;
    const char *input_filename;
    unsigned page_size;
    unsigned max_memory;
} arguments;

typedef struct {
    unsigned page_reads;
    unsigned page_faults;
    unsigned page_writes;
    unsigned page_hits;
} info_table;

void parse_args(int argc, char **argv);
void read_input_file();
void simulate_access(unsigned address, char operation);
void init_page_table();
unsigned *access_page(unsigned address);
void usage(const char *program_name);
void write_results();