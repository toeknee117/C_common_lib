//  Developed Libraries
#include "circular_queue.h"


static uint8_t errorArray[120] = {0};
/*
    Goal of Function:
    Initialize the struct for the queue
*/
circular_queue_t *queueInit(uint32_t capacity) {
    circular_queue_t *queue_info = (circular_queue_t*) malloc(sizeof(circular_queue_t));
    queue_info->max_capacity = capacity;
    queue_info->front = 0;
    queue_info->rear = 0;
    queue_info->size = 0;
    if (pthread_mutex_init(&queue_info->queueLock, NULL) != 0) {
        snprintf(errorArray, sizeof(errorArray), "%s: Error Initializing Queue Mutex Lock\n", __FUNCTION__);
        perror(errorArray);
        exit(0);
    }
    if (pthread_cond_init(&queue_info->queueCond, NULL) != 0) {
        snprintf(errorArray, sizeof(errorArray), "%s: Error Initializing Queue Condition Lock\n", __FUNCTION__);
        perror(errorArray);
        exit(0);
    }
    queue_info->queue = calloc(capacity, sizeof(uint8_t));
    return queue_info;
}

/*
    Goal of Function:
    Destroy the struct for the queue
*/
void queueDestroy(circular_queue_t *queue_info) {
    free(queue_info->queue);
    pthread_mutex_destroy(&queue_info->queueLock);
    pthread_cond_destroy(&queue_info->queueCond);
    free(queue_info);
}

/*
    Goal of Function:
    Add data to the queue
*/
void enqueue(circular_queue_t *queue_info, uint8_t data) {
    if (queue_info->size == queue_info->max_capacity) {
        printf("%s: Queue Overflow, not performing\n", __FUNCTION__);
        return;
    }
    queue_info->rear = (queue_info->front + queue_info->size) % queue_info->max_capacity;
    queue_info->queue[queue_info->rear] = data;
    queue_info->size++;
}

/*
    Goal of Function:
    Enqueue chunk
*/
void enqueueChunk(circular_queue_t *queue_info, uint8_t *data, uint32_t amount) {
    if ((queue_info->max_capacity - queue_info->size) < amount) {
        printf("%s: Queue Will Overflow, not performing\n", __FUNCTION__);
        return;
    }
    for (uint32_t i = 0; i < amount; i++) {
        enqueue(queue_info, data[i]);
    }
}

/*
    Goal of Function:
    Remove data to the queue
*/
uint8_t dequeue(circular_queue_t *queue_info, uint8_t *isThereData) {
    if (queue_info->size == 0) {
        *isThereData = 0;
        return 0;
    }
    *isThereData = 1;
    int32_t data = queue_info->queue[queue_info->front];
    queue_info->front = (queue_info->front + 1) % queue_info->max_capacity;
    queue_info->size--;
    return data;
}

/*
    Goal of Function:
    Dequeue all data
*/
uint8_t *dequeueChunk(circular_queue_t *queue_info, uint32_t *amount) {
    if (queue_info->size == 0) {
        *amount = 0;
        return 0;
    }
    *amount = queue_info->size;
    uint8_t *allData = calloc(queue_info->size, sizeof(uint8_t));
    uint8_t isThereData = 1;
    uint32_t dataByte = 0;
    while (isThereData) {
        allData[dataByte++] = dequeue(queue_info, &isThereData);
    }

    return allData;
}

