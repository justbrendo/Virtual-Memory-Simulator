#!/bin/bash

# List of page replacement algorithms
algorithms=("lru" "2a" "fifo" "random")

# List of page sizes in KB
page_sizes=(2 4 8 16 32 64)

# List of memory sizes in KB
mem_sizes=(128 256 512 1024 2048 4096 8192 16384)

# List of file names
files=("compilador.log")

# Function to run the program with the given parameters
run_program() {
    local algorithm=$1
    local file=$2
    local page_size=$3
    local max_memory=$4
    local output_file="results/${algorithm}_${file}_${page_size}KB_${max_memory}KB.txt"
    ./main.out $algorithm $file $page_size $max_memory
}

# Run the program for all combinations of algorithms, page sizes, and memory sizes
for algorithm in "${algorithms[@]}"; do
    for file in "${files[@]}"; do
        # Situation 1: Vary memory size with a fixed page size of 4 KB
        for mem_size in "${mem_sizes[@]}"; do
            run_program $algorithm $file 4 $mem_size
        done
        # Situation 2: Vary page size with a fixed memory size of 16384 KB
        for page_size in "${page_sizes[@]}"; do
            run_program $algorithm $file $page_size 16384
        done
    done
done

echo "Executions completed. Results stored in the output.txt."