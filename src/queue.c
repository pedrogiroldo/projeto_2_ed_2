#include "queue.h"
#include <stdio.h>

// Internal structure definitions - only visible in implementation
typedef struct QueueNode {
  void *data;
  struct QueueNode *next;
} QueueNode;

// Complete Queue structure definition
struct Queue {
  QueueNode *front; // First element in queue
  QueueNode *rear;  // Last element in queue
  int size;         // Current queue size
};

/**
 * Creates a new empty queue
 * @return Pointer to new queue or NULL on error
 */
Queue queue_create(void) {
  struct Queue *queue = (struct Queue *)malloc(sizeof(struct Queue));
  if (queue == NULL) {
    return NULL;
  }

  queue->front = NULL;
  queue->rear = NULL;
  queue->size = 0;

  return (Queue)queue;
}

/**
 * Destroys the queue and frees all associated memory
 * @param queue Pointer to queue to be destroyed
 */
void queue_destroy(Queue queue) {
  if (queue == NULL) {
    return;
  }

  queue_clear(queue);
  free(queue);
}

/**
 * Adds an element to the end of the queue
 * @param queue Pointer to the queue
 * @param data Data to be added
 * @return true on success, false on error
 */
bool queue_enqueue(Queue queue, void *data) {
  if (queue == NULL) {
    return false;
  }

  struct Queue *q = (struct Queue *)queue;
  QueueNode *new_node = (QueueNode *)malloc(sizeof(QueueNode));
  if (new_node == NULL) {
    return false;
  }

  new_node->data = data;
  new_node->next = NULL;

  if (queue_is_empty(queue)) {
    q->front = new_node;
    q->rear = new_node;
  } else {
    q->rear->next = new_node;
    q->rear = new_node;
  }

  q->size++;
  return true;
}

/**
 * Removes and returns the first element from the queue
 * @param queue Pointer to the queue
 * @return Data from first element or NULL if queue is empty
 */
void *queue_dequeue(Queue queue) {
  if (queue == NULL || queue_is_empty(queue)) {
    return NULL;
  }

  struct Queue *q = (struct Queue *)queue;
  QueueNode *node_to_remove = q->front;
  void *data = node_to_remove->data;

  q->front = q->front->next;

  // If queue became empty, update rear as well
  if (q->front == NULL) {
    q->rear = NULL;
  }

  free(node_to_remove);
  q->size--;

  return data;
}

/**
 * Returns the first element from the queue without removing it
 * @param queue Pointer to the queue
 * @return Data from first element or NULL if queue is empty
 */
void *queue_peek(Queue queue) {
  if (queue == NULL || queue_is_empty(queue)) {
    return NULL;
  }

  struct Queue *q = (struct Queue *)queue;
  return q->front->data;
}

/**
 * Checks if the queue is empty
 * @param queue Pointer to the queue
 * @return true if empty, false otherwise
 */
bool queue_is_empty(Queue queue) {
  if (queue == NULL) {
    return true;
  }
  struct Queue *q = (struct Queue *)queue;
  return (q->front == NULL);
}

/**
 * Returns the current size of the queue
 * @param queue Pointer to the queue
 * @return Number of elements in the queue
 */
int queue_size(Queue queue) {
  if (queue == NULL) {
    return 0;
  }

  struct Queue *q = (struct Queue *)queue;
  return q->size;
}

/**
 * Removes all elements from the queue
 * @param queue Pointer to the queue
 */
void queue_clear(Queue queue) {
  if (queue == NULL) {
    return;
  }

  while (!queue_is_empty(queue)) {
    queue_dequeue(queue);
  }
}