#include "fifo.h"  // Assuming fifo.h defines any necessary structures and prototypes
#include <stdlib.h> // For rand() and srand()
#include <time.h>   // For srand() initialization

#include "random.h"

// Queue structure for Random Replacement
typedef struct {
    int *arr;
    unsigned capacity;
    unsigned size;
} RandomQueue;

RandomQueue *random_queue;

// Function to create a Random Queue
RandomQueue* createRandomQueue(unsigned capacity) {
    RandomQueue* queue = (RandomQueue*) malloc(sizeof(RandomQueue));
    if (queue == NULL) {
        perror("Error allocating memory for random queue");
        exit(EXIT_FAILURE);
    }
    queue->capacity = capacity;
    queue->size = 0;
    queue->arr = (int*) malloc(queue->capacity * sizeof(int));
    if (queue->arr == NULL) {
        perror("Error allocating memory for random queue array");
        exit(EXIT_FAILURE);
    }
    return queue;
}

// Function to check if Random Queue is full
unsigned isRandomQueueFull(RandomQueue* queue) {
    return queue->size == queue->capacity;
}

// Function to add an item to the Random Queue
void enqueueRandom(RandomQueue* queue, int item) {
    if (isRandomQueueFull(queue)) {
        fprintf(stderr, "Random Queue is full. Cannot enqueue item %d\n", item);
        return;
    }
    queue->arr[queue->size++] = item;
}

// Function to remove an item from the Random Queue
unsigned dequeueRandom(RandomQueue* queue) {
    if (queue->size == 0) {
        return -1;
    }
    // Generate a random index to select a page to evict
    int random_index = rand() % queue->size;
    int item = queue->arr[random_index];
    // Replace the selected item with the last item in the queue
    queue->arr[random_index] = queue->arr[queue->size - 1];
    queue->size--;
    return item;
}

void random_init(arguments *args) {
    // Create Random queue
    random_queue = createRandomQueue(args->max_memory / args->page_size);

    // Initialize random number generator
    srand(time(NULL));
}

void random_algorithm(unsigned address, unsigned num_frames, unsigned *used_frames) {
    // Check if there's space in memory to allocate a new page
    if (*used_frames < num_frames) {
        *access_page(address) = 1;
        enqueueRandom(random_queue, address);
        *used_frames = *used_frames + 1;
    } else {
        // Dequeue a randomly selected page
        int page_to_evict = dequeueRandom(random_queue);
        *access_page(page_to_evict) = 0;
        *access_page(address) = 1;
        // Enqueue the new page address
        enqueueRandom(random_queue, address);
    }
}
