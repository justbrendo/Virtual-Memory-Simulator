#include "fifo.h"  // Assuming fifo.h defines any necessary structures and prototypes

// Queue structure for FIFO
typedef struct {
    int *arr;
    int front, rear;
    unsigned capacity;
} Queue;

Queue *fifo_queue;

// Function to create a queue
Queue* createQueue(unsigned capacity) {
    Queue* queue = (Queue*) malloc(sizeof(Queue));
    if (queue == NULL) {
        perror("Error allocating memory for queue");
        exit(EXIT_FAILURE);
    }
    queue->capacity = capacity;
    queue->front = queue->rear = -1;
    queue->arr = (int*) malloc(queue->capacity * sizeof(int));
    if (queue->arr == NULL) {
        perror("Error allocating memory for queue array");
        exit(EXIT_FAILURE);
    }
    return queue;
}

// Function to check if queue is full
unsigned isFull(Queue* queue) {
    return (queue->rear + 1) % queue->capacity == queue->front;
}

// Function to check if queue is empty
unsigned isEmpty(Queue* queue) {
    return queue->front == -1;
}

// Function to add an item to the queue
void enqueue(Queue* queue, int item) {
    if (isFull(queue)) {
        return;
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->arr[queue->rear] = item;
    if (queue->front == -1)
        queue->front = queue->rear;
}

// Function to remove an item from the queue
unsigned dequeue(Queue* queue) {
    if (isEmpty(queue)) {
        return -1;
    }
    int item = queue->arr[queue->front];
    if (queue->front == queue->rear)
        queue->front = queue->rear = -1;
    else
        queue->front = (queue->front + 1) % queue->capacity;
    return item;
}

void fifo_init(arguments *args) {
     // Create FIFO queue
    fifo_queue = createQueue(args->max_memory / args->page_size);
    unsigned num_frames = 0; // Track number of currently occupied page frames
}

void fifo_algorithm(unsigned address, unsigned num_frames, unsigned *used_frames) {
   // Check if there's space in memory to allocate a new page
   if(*used_frames < num_frames) {
        *access_page(address) = 1;
        enqueue(fifo_queue, address);
        *used_frames = *used_frames + 1;
   } else {
    int oldest_page = dequeue(fifo_queue);
    *access_page(oldest_page) = 0;
    *access_page(address) = 1;
    enqueue(fifo_queue, address);
   }
}