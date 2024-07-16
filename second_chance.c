#include "fifo.h"  // Assuming fifo.h defines any necessary structures and prototypes
#include <stdlib.h> // For malloc(), free()
#include <stdio.h>  // For printf(), fprintf(), perror()

// Queue structure for Second Chance (Clock) Algorithm
typedef struct {
    int *arr;
    unsigned *bit;
    unsigned capacity;
    unsigned size;
    unsigned head;  // Pointer to the current position in the circular queue
} SecondChanceQueue;

SecondChanceQueue *second_chance_queue;

// Function to create a Second Chance Queue
SecondChanceQueue* createSecondChanceQueue(unsigned capacity) {
    SecondChanceQueue* queue = (SecondChanceQueue*) malloc(sizeof(SecondChanceQueue));
    if (queue == NULL) {
        perror("Error allocating memory for Second Chance queue");
        exit(EXIT_FAILURE);
    }
    queue->capacity = capacity;
    queue->size = 0;
    queue->head = 0;
    queue->arr = (int*) malloc(queue->capacity * sizeof(int));
    queue->bit = (unsigned*) malloc(queue->capacity * sizeof(unsigned));
    if (queue->arr == NULL || queue->bit == NULL) {
        perror("Error allocating memory for Second Chance queue array");
        exit(EXIT_FAILURE);
    }
    // Initialize all reference bits to 0
    for (unsigned i = 0; i < queue->capacity; ++i) {
        queue->bit[i] = 0;
    }
    return queue;
}

// Function to check if Second Chance Queue is full
unsigned isSecondChanceQueueFull(SecondChanceQueue* queue) {
    return queue->size == queue->capacity;
}

// Function to add an item to the Second Chance Queue
void enqueueSecondChance(SecondChanceQueue* queue, int item) {
    if (isSecondChanceQueueFull(queue)) {
        fprintf(stderr, "Second Chance Queue is full. Cannot enqueue item %d\n", item);
        return;
    }
    queue->arr[(queue->head + queue->size) % queue->capacity] = item;
    queue->bit[(queue->head + queue->size) % queue->capacity] = 0; // Initialize reference bit to 0
    queue->size++;
}

// Function to remove an item from the Second Chance Queue
unsigned dequeueSecondChance(SecondChanceQueue* queue) {
    if (queue->size == 0) {
        return -1;
    }
    // Use a circular scan to find the page to evict
    while (1) {
        if (queue->bit[queue->head] == 0) {
            // If reference bit is 0, evict this page
            unsigned item = queue->arr[queue->head];
            // Shift remaining elements to the left
            for (unsigned i = queue->head; i < queue->size - 1; ++i) {
                queue->arr[i] = queue->arr[i + 1];
                queue->bit[i] = queue->bit[i + 1];
            }
            queue->size--;
            return item;
        } else {
            // Set reference bit to 0 and move head pointer
            queue->bit[queue->head] = 0;
            queue->head = (queue->head + 1) % queue->capacity;
        }
    }
}

void second_chance_init(arguments *args) {
    // Create Second Chance queue
    second_chance_queue = createSecondChanceQueue(args->max_memory / args->page_size);
}

void second_chance_algorithm(unsigned address, unsigned num_frames, unsigned *used_frames) {
    // Check if there's space in memory to allocate a new page
    if (*used_frames < num_frames) {
        *access_page(address) = 1;
        enqueueSecondChance(second_chance_queue, address);
        *used_frames = *used_frames + 1;
    } else {
        // Dequeue a page using Second Chance (Clock) algorithm
        unsigned evicted_page = dequeueSecondChance(second_chance_queue);
        *access_page(evicted_page) = 0;
        *access_page(address) = 1;
        // Enqueue the new page address
        enqueueSecondChance(second_chance_queue, address);
    }
}
