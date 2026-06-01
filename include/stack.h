/**
 * @file stack.h
 * @brief Stack ADT implementation
 *
 * This module provides an abstract data type for a stack data structure.
 * The stack uses opaque pointers (void*) to maintain encapsulation.
 * All functions needed to create, manipulate, and query a stack are provided.
 */

#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief Opaque pointer type for stack instances
 */
typedef void *Stack;

/**
 * @brief Creates a new empty stack instance
 * @return Pointer to new stack or NULL on error
 */
Stack stack_create(void);

/**
 * @brief Destroys a stack instance and frees all memory
 * @param stack Stack instance to destroy
 */
void stack_destroy(Stack stack);

/**
 * @brief Pushes an element onto the stack
 * @param stack Stack instance
 * @param data Pointer to data to push
 * @return true if successful, false otherwise
 */
bool stack_push(Stack stack, void *data);

/**
 * @brief Pops and returns the top element from the stack
 * @param stack Stack instance
 * @return Pointer to popped data or NULL if stack is empty
 */
void *stack_pop(Stack stack);

/**
 * @brief Returns the top element without removing it
 * @param stack Stack instance
 * @return Pointer to top element or NULL if stack is empty
 */
void *stack_peek(Stack stack);

/**
 * @brief Returns the element at the specified index
 * @param stack Stack instance
 * @param index Index from top (0 = top element)
 * @return Pointer to element at index or NULL if invalid index
 */
void *stack_peek_at(Stack stack, int index);

/**
 * @brief Checks if the stack is empty
 * @param stack Stack instance
 * @return true if stack is empty, false otherwise
 */
bool stack_is_empty(Stack stack);

/**
 * @brief Gets the number of elements in the stack
 * @param stack Stack instance
 * @return Number of elements in stack
 */
int stack_size(Stack stack);

/**
 * @brief Removes all elements from the stack without destroying it
 * @param stack Stack instance
 */
void stack_clear(Stack stack);

#endif // STACK_H
