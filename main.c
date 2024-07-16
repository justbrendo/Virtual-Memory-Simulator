#include "main.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "algorithms.h"

#define GET_LSB(x, num) ((num) & ((1 << (x)) - 1))

typedef struct {
    unsigned virtual_page;
    unsigned valid;
} inverted_page_table_entry;

arguments args;
info_table info = {0, 0, 0, 0};
algorithm_func_t chosen_algorithm = NULL;
algorithm_func_t init_algorithm = NULL;
FILE *input_file, *output_file;
unsigned s, num_frames;
unsigned used_frames = 0;
const char *output_filename = "output.txt";
int hierarchy = 1;
unsigned *page_table;
unsigned **outer_page_table;
unsigned ***outer_outer_page_table;
unsigned bits_L1, bits_L2, bits_L3;
inverted_page_table_entry *inverted_page_table;

void reset() {
    info.page_faults = 0;
    info.page_hits = 0;
    info.page_reads = 0;
    info.page_writes = 0;
}

int main(int argc, char **argv) {
    parse_args(argc, argv);

    // First run using non hierarchical page table
    init_page_table();
    init_algorithm(&args);
    read_input_file();
    write_results();
    reset();

    // Free the allocated memory for the page table
    free(page_table);

    // Reset page table and run using hierarchical page table of 2 levels
    hierarchy = 2;
    init_page_table();
    init_algorithm(&args);
    read_input_file();
    write_results();
    reset();

    // Free the allocated memory for the outer page table
    for (unsigned i = 0; i < pow(2, (32 - s) / 2); i++) {
        free(outer_page_table[i]);
    }
    free(outer_page_table);

    // Reset page table and run using hierarchical page table of 3 levels
    hierarchy = 3;
    init_page_table();
    init_algorithm(&args);
    read_input_file();
    write_results();
    reset();

    // Free the allocated memory for the outer outer page table
    for (unsigned i = 0; i < pow(2, (32 - s) / 3); i++) {
        for (unsigned j = 0; j < pow(2, (32 - s) / 3); j++) {
            free(outer_outer_page_table[i][j]);
        }
        free(outer_outer_page_table[i]);
    }
    free(outer_outer_page_table);

    // Reset page table and run using inverted page table
    hierarchy = -1;
    init_page_table();
    init_algorithm(&args);
    read_input_file();
    write_results();
    reset();

    return 0;
}

void parse_args(int argc, char **argv) {
    if (argc != 5) {
        usage(argv[0]);
    }

    args.algorithm = argv[1];
    args.input_filename = argv[2];
    args.page_size = atoi(argv[3]);
    args.max_memory = atoi(argv[4]);

    // Validate the page replacement algorithm argument
    for (int i = 0; algorithms[i].name != NULL; i++) {
        if (strcmp(args.algorithm, algorithms[i].name) == 0) {
            chosen_algorithm = algorithms[i].func;
            init_algorithm = algorithms[i].init;
            break;
        }
    }
    if (chosen_algorithm == NULL) {
        printf("Error: Invalid algorithm. Use one of lru, 2a, fifo, or random.\n");
        usage(argv[0]);
    }

    // Validate the page size argument
    if (args.page_size < 2 || args.page_size > 64) {
        printf("Error: Page size must be an integer between 2 and 64.\n");
        usage(argv[0]);
    }

    // Validate the max memory argument
    if (args.max_memory < 128 || args.max_memory > 16384) {
        printf("Error: Max memory must be an integer between 128 and 16384.\n");
        usage(argv[0]);
    }

    // Calculate s (number of bits for page offset)
    s = 0;
    unsigned tmp = 1024 * args.page_size;
    while (tmp > 1) {
        tmp = tmp >> 1;
        s++;
    }

    // Calculate number of frames based on max memory and page size
    num_frames = args.max_memory / args.page_size;
}

void init_page_table() {
    if (hierarchy == 1) {
        // Calculate number of pages and allocate the main page table
        unsigned num_pages = pow(2, 32 - s);
        page_table = (int *)malloc(num_pages * sizeof(int));

        // Initialize main page table with 0 (invalid page)
        for (unsigned i = 0; i < num_pages; i++) {
            page_table[i] = 0;
        }

    } else if (hierarchy == 2) {
        // Allocate the Level 1 page table
        bits_L1 = (32 - s) / 2;  // Half of the address space
        unsigned num_pages_L1 = pow(2, bits_L1);
        outer_page_table = (int **)malloc(num_pages_L1 * sizeof(int *));

        bits_L2 = (32 - s - bits_L1);  // Half of the address space
        unsigned num_pages_L2 = pow(2, bits_L2);
        // Alocate Level 2 page tables
        for (unsigned i = 0; i < num_pages_L1; i++) {
            outer_page_table[i] = (int *)malloc(num_pages_L2 * sizeof(int));

            // Initialize Level 2 page tables with 0 (invalid page)
            for (unsigned j = 0; j < num_pages_L2; j++) {
                outer_page_table[i][j] = 0;
            }
        }
    } else if (hierarchy == 3) {
        // Allocate the Level 1 page table
        bits_L1 = (32 - s) / 3;  // One third of the address space
        unsigned num_pages_L1 = pow(2, bits_L1);
        outer_outer_page_table = (int **)malloc(num_pages_L1 * sizeof(int *));

        // Allocate Level 2 page tables
        bits_L2 = (32 - s) / 3;  // One third of the address space
        unsigned num_pages_L2 = pow(2, bits_L2);

        // Allocate Level 3 page tables
        bits_L3 = (32 - s - bits_L1 - bits_L2);  // One third of the address space
        unsigned num_pages_L3 = pow(2, bits_L3);

        for (unsigned i = 0; i < num_pages_L1; i++) {
            outer_outer_page_table[i] = (int **)malloc(num_pages_L2 * sizeof(int *));

            for (unsigned j = 0; j < num_pages_L2; j++) {
                outer_outer_page_table[i][j] = (int *)malloc(num_pages_L3 * sizeof(int));

                // Initialize Level 3 page tables with 0 (invalid page)
                for (unsigned k = 0; k < num_pages_L3; k++) {
                    outer_outer_page_table[i][j][k] = 0;
                }
            }
        }
    } else if (hierarchy == -1) {
        // Allocate inverted page table
        inverted_page_table = (inverted_page_table_entry *)malloc(pow(2, 32 - s) * sizeof(inverted_page_table_entry));
        // Initialize inverted page table entries
        for (unsigned i = 0; i < pow(2, 32 - s); i++) {
            inverted_page_table[i].virtual_page = 0;  // Invalid virtual page initially
            inverted_page_table[i].valid = 0;         // Mark as unused
        }
    }
}

void read_input_file() {
    // Variables for reading the input file
    char line[20];
    unsigned addr;
    char operation;

    // Open the file
    // Validate the input File
    input_file = fopen(args.input_filename, "r");
    if (input_file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    // Read each line of the file
    while (fgets(line, sizeof(line), input_file) != NULL) {
        // Parse the line to extract the hexadecimal number and the operation (r/w)
        if (sscanf(line, "%x %c", &addr, &operation) == 2) {
            simulate_access(addr, operation);
        }
    }

    fclose(input_file);
}

void simulate_access(unsigned address, char operation) {
    if (*access_page(address) == 0) {
        info.page_faults++;
        // Load the page into memory
        chosen_algorithm(address, num_frames, &used_frames);
    } else {
        info.page_hits++;
    }

    if (operation == 'R') {
        info.page_reads++;
    } else if (operation == 'W') {
        info.page_writes++;
    }
}

unsigned *access_page(unsigned address) {
    switch (hierarchy) {
        case 1:
            // Calculate index into the main page table
            return &page_table[address >> s];

        case 2:
            // Calculate indices for Level 1 and Level 2 page tables
            unsigned idx_L1 = GET_LSB(bits_L1, address);
            unsigned idx_L2 = GET_LSB(bits_L2, address >> bits_L1);

            // Access the Level 2 page table entry
            return &outer_page_table[idx_L1][idx_L2];

        case 3:
            // Calculate indices for Level 1, Level 2, and Level 3 page tables
            unsigned idx_L1_3 = GET_LSB(bits_L1, address);
            unsigned idx_L2_3 = GET_LSB(bits_L2, address >> bits_L1);
            unsigned idx_L3_3 = GET_LSB(bits_L3, address >> (bits_L1 + bits_L2));

            // Access the Level 3 page table entry
            return &outer_outer_page_table[idx_L1_3][idx_L2_3][idx_L3_3];

        case -1:
            // Calculate frame number from address
            unsigned frame_number = address >> s;
            // Access inverted page table entry
            inverted_page_table_entry *entry = &inverted_page_table[frame_number];
            // Return a pointer to the valid/invalid bit in the inverted page table
            return &entry->valid;

        default:
            // This should not happen if hierarchy is properly initialized
            printf("Error: Invalid hierarchy value\n");
            exit(1);
    }
}

void write_results() {
    // Open output file for writing
    output_file = fopen(output_filename, "a");
    if (output_file == NULL) {
        perror("Error opening output file");
        exit(1);
    }

    // Write relevant information to the output file
    fprintf(output_file, "Algorithm: %s\n", args.algorithm);
    fprintf(output_file, "Hierarchy: %d\n", hierarchy);
    fprintf(output_file, "File Name: %s\n", args.input_filename);
    fprintf(output_file, "Page Size: %d\n", args.page_size);
    fprintf(output_file, "Max Memory: %d\n", args.max_memory);
    fprintf(output_file, "Page Reads: %d\n", info.page_reads);
    fprintf(output_file, "Page Writes: %d\n", info.page_writes);
    fprintf(output_file, "Page Faults: %d\n", info.page_faults);
    fprintf(output_file, "Page Hits: %d\n\n", info.page_hits);

    fclose(output_file);
}

void usage(const char *program_name) {
    printf("Usage: %s <lru|2a|fifo|random> <file_name> <page_size> <max_memory>\n", program_name);
    exit(1);
}