#include "fifo.h"  // Assuming fifo.h defines any necessary structures and prototypes
#include <stdlib.h> // For malloc(), free()
#include <stdio.h>  // For printf(), fprintf(), perror()
#include <limits.h> // For INT_MAX

// Queue structure for LRU (Least Recently Used)
typedef struct {
    int *arr;
    int *timestamps;
    unsigned capacity;
    unsigned size;
} LRUQueue;

LRUQueue *lru_queue;

// Function to create an LRU Queue
LRUQueue* createLRUQueue(unsigned capacity) {
    LRUQueue* queue = (LRUQueue*) malloc(sizeof(LRUQueue));
    if (queue == NULL) {
        perror("Error allocating memory for LRU queue");
        exit(EXIT_FAILURE);
    }
    queue->capacity = capacity;
    queue->size = 0;
    queue->arr = (int*) malloc(queue->capacity * sizeof(int));
    queue->timestamps = (int*) malloc(queue->capacity * sizeof(int));
    if (queue->arr == NULL || queue->timestamps == NULL) {
        perror("Error allocating memory for LRU queue array");
        exit(EXIT_FAILURE);
    }
    // Initialize timestamps to a large negative number (indicating not accessed)
    for (unsigned i = 0; i < queue->capacity; ++i) {
        queue->timestamps[i] = INT_MIN;
    }
    return queue;
}

// Function to check if LRU Queue is full
unsigned isLRUQueueFull(LRUQueue* queue) {
    return queue->size == queue->capacity;
}

// Function to find the index of the least recently used page
int findLRUIndex(LRUQueue* queue) {
    int min_index = 0;
    int min_timestamp = queue->timestamps[0];
    for (unsigned i = 1; i < queue->size; ++i) {
        if (queue->timestamps[i] < min_timestamp) {
            min_timestamp = queue->timestamps[i];
            min_index = i;
        }
    }
    return min_index;
}

// Function to add an item to the LRU Queue
void enqueueLRU(LRUQueue* queue, int item) {
    if (isLRUQueueFull(queue)) {
        // If the queue is full, evict the least recently used page
        int lru_index = findLRUIndex(queue);
        queue->arr[lru_index] = item;
        queue->timestamps[lru_index] = 0; // Reset timestamp
    } else {
        // Add the item to the end of the queue
        queue->arr[queue->size++] = item;
    }
}

// Function to remove an item from the LRU Queue
unsigned dequeueLRU(LRUQueue* queue) {
    if (queue->size == 0) {
        return -1;
    }
    // Find the least recently used page (oldest timestamp)
    int lru_index = findLRUIndex(queue);
    int item = queue->arr[lru_index];
    // Shift remaining elements to the left
    for (unsigned i = lru_index; i < queue->size - 1; ++i) {
        queue->arr[i] = queue->arr[i + 1];
        queue->timestamps[i] = queue->timestamps[i + 1];
    }
    queue->size--;
    return item;
}

void lru_init(arguments *args) {
    // Create LRU queue
    lru_queue = createLRUQueue(args->max_memory / args->page_size);
}

void lru_algorithm(unsigned address, unsigned num_frames, unsigned *used_frames) {
    // Check if there's space in memory to allocate a new page
    if (*used_frames < num_frames) {
        *access_page(address) = 1;
        enqueueLRU(lru_queue, address);
        *used_frames = *used_frames + 1;
    } else {
        // Dequeue the least recently used page
        int lru_page = dequeueLRU(lru_queue);
        *access_page(lru_page) = 0;
        *access_page(address) = 1;
        // Enqueue the new page address
        enqueueLRU(lru_queue, address);
    }
}
