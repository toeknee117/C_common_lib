#pragma once
#pragma(push, 1)
#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

//  Standard Libraries
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>

//  Circular Queue Struct
typedef struct _circular_queue_t {
    uint32_t max_capacity;
    uint32_t front;
    uint32_t rear;
    uint32_t size;
    pthread_mutex_t queueLock;
    pthread_cond_t queueCond;
    uint8_t *queue;
} circular_queue_t, *p_circular_queue_t;

//  Declare Functions
circular_queue_t *queueInit(uint32_t capacity);
void queueDestroy(circular_queue_t *queue);
void enqueue(circular_queue_t *queue, uint8_t data);
void enqueueChunk(circular_queue_t *queue_info, uint8_t *data, uint32_t amount);
uint8_t dequeue(circular_queue_t *queue_info, uint8_t *isThereData);
uint8_t *dequeueChunk(circular_queue_t *queue_info, uint32_t *amount);

#endif