/**
 * @file queue.h
 * @brief Queue ADT implementation
 *
 * This module provides an abstract data type for a queue data structure.
 * The queue uses opaque pointers (void*) to maintain encapsulation.
 * All functions needed to create, manipulate, and query a queue are provided.
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief Opaque pointer type for queue instances
 */
typedef void *Queue;

/**
 * @brief Creates a new empty queue instance
 * @return Pointer to new queue or NULL on error
 */
Queue queue_create(void);

/**
 * @brief Destroys a queue instance and frees all memory
 * @param queue Queue instance to destroy
 */
void queue_destroy(Queue queue);

/**
 * @brief Adds an element to the rear of the queue
 * @param queue Queue instance
 * @param data Pointer to data to enqueue
 * @return true if successful, false otherwise
 */
bool queue_enqueue(Queue queue, void *data);

/**
 * @brief Removes and returns the front element from the queue
 * @param queue Queue instance
 * @return Pointer to dequeued data or NULL if queue is empty
 */
void *queue_dequeue(Queue queue);

/**
 * @brief Returns the front element without removing it
 * @param queue Queue instance
 * @return Pointer to front element or NULL if queue is empty
 */
void *queue_peek(Queue queue);

/**
 * @brief Checks if the queue is empty
 * @param queue Queue instance
 * @return true if queue is empty, false otherwise
 */
bool queue_is_empty(Queue queue);

/**
 * @brief Gets the number of elements in the queue
 * @param queue Queue instance
 * @return Number of elements in queue
 */
int queue_size(Queue queue);

/**
 * @brief Removes all elements from the queue without destroying it
 * @param queue Queue instance
 */
void queue_clear(Queue queue);

#endif // QUEUE_H