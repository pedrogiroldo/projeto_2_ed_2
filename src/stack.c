#include "stack.h"
#include <stdio.h>

// Internal structure definitions - only visible in implementation
typedef struct StackNode {
  void *data;
  struct StackNode *next;
} StackNode;

// Complete Stack structure definition
struct Stack {
  StackNode *top; // Top element in stack
  int size;       // Current stack size
};

/**
 * Creates a new empty stack
 * @return Pointer to new stack or NULL on error
 */
Stack stack_create(void) {
  struct Stack *stack = (struct Stack *)malloc(sizeof(struct Stack));
  if (stack == NULL) {
    return NULL;
  }

  stack->top = NULL;
  stack->size = 0;

  return (Stack)stack;
}

/**
 * Destroys the stack and frees all associated memory
 * @param stack Pointer to stack to be destroyed
 */
void stack_destroy(Stack stack) {
  if (stack == NULL) {
    return;
  }

  stack_clear(stack);
  free(stack);
}

/**
 * Adds an element to the top of the stack
 * @param stack Pointer to the stack
 * @param data Data to be added
 * @return true on success, false on error
 */
bool stack_push(Stack stack, void *data) {
  if (stack == NULL) {
    return false;
  }

  struct Stack *s = (struct Stack *)stack;
  StackNode *new_node = (StackNode *)malloc(sizeof(StackNode));
  if (new_node == NULL) {
    return false;
  }

  new_node->data = data;
  new_node->next = s->top;
  s->top = new_node;
  s->size++;

  return true;
}

/**
 * Removes and returns the top element from the stack
 * @param stack Pointer to the stack
 * @return Data from top element or NULL if stack is empty
 */
void *stack_pop(Stack stack) {
  if (stack == NULL || stack_is_empty(stack)) {
    return NULL;
  }

  struct Stack *s = (struct Stack *)stack;
  StackNode *node_to_remove = s->top;
  void *data = node_to_remove->data;

  s->top = s->top->next;
  free(node_to_remove);
  s->size--;

  return data;
}

/**
 * Returns the top element from the stack without removing it
 * @param stack Pointer to the stack
 * @return Data from top element or NULL if stack is empty
 */
void *stack_peek(Stack stack) {
  if (stack == NULL || stack_is_empty(stack)) {
    return NULL;
  }

  struct Stack *s = (struct Stack *)stack;
  return s->top->data;
}

/**
 * Returns the element at the specified index from the stack without removing it
 * @param stack Pointer to the stack
 * @param index Index of the element (0-based, 0 is top of stack)
 * @return Data from element at index or NULL if invalid index or stack is empty
 */
void *stack_peek_at(Stack stack, int index) {
  if (stack == NULL || stack_is_empty(stack) || index < 0) {
    return NULL;
  }

  struct Stack *s = (struct Stack *)stack;
  if (index >= s->size) {
    return NULL;
  }

  StackNode *current = s->top;
  for (int i = 0; i < index; i++) {
    if (current == NULL) {
      return NULL;
    }
    current = current->next;
  }

  return current->data;
}

/**
 * Checks if the stack is empty
 * @param stack Pointer to the stack
 * @return true if empty, false otherwise
 */
bool stack_is_empty(Stack stack) {
  if (stack == NULL) {
    return true;
  }
  struct Stack *s = (struct Stack *)stack;
  return (s->top == NULL);
}

/**
 * Returns the current size of the stack
 * @param stack Pointer to the stack
 * @return Number of elements in the stack
 */
int stack_size(Stack stack) {
  if (stack == NULL) {
    return 0;
  }

  struct Stack *s = (struct Stack *)stack;
  return s->size;
}

/**
 * Removes all elements from the stack
 * @param stack Pointer to the stack
 */
void stack_clear(Stack stack) {
  if (stack == NULL) {
    return;
  }

  while (!stack_is_empty(stack)) {
    stack_pop(stack);
  }
}
