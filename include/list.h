/**
 * @file list.h
 * @brief List ADT implementation
 *
 * This module provides an abstract data type for a doubly-linked list data
 * structure. The list uses opaque pointers (void*) to maintain encapsulation.
 * All functions needed to create, manipulate, and query a list are provided.
 */

#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief Opaque pointer type for list instances
 */
typedef void *List;

/**
 * @brief Creates a new empty list instance
 * @return Pointer to new list or NULL on error
 */
List list_create(void);

/**
 * @brief Destroys a list instance and frees all memory
 * @param list List instance to destroy
 */
void list_destroy(List list);

/**
 * @brief Adds an element to the end of the list
 * @param list List instance
 * @param data Pointer to data to insert
 * @return true if successful, false otherwise
 */
bool list_insert_back(List list, void *data);

/**
 * @brief Adds an element to the beginning of the list
 * @param list List instance
 * @param data Pointer to data to insert
 * @return true if successful, false otherwise
 */
bool list_insert_front(List list, void *data);

/**
 * @brief Removes a specific element from the list
 * @param list List instance
 * @param data Pointer to data to remove
 * @return true if element was found and removed, false otherwise
 */
bool list_remove(List list, void *data);

/**
 * @brief Gets the element at the specified index
 * @param list List instance
 * @param index Zero-based index of element to retrieve
 * @return Pointer to element at index or NULL if index is out of bounds
 */
void *list_get(List list, int index);

/**
 * @brief Gets the first element in the list
 * @param list List instance
 * @return Pointer to first element or NULL if list is empty
 */
void *list_get_first(List list);

/**
 * @brief Gets the last element in the list
 * @param list List instance
 * @return Pointer to last element or NULL if list is empty
 */
void *list_get_last(List list);

/**
 * @brief Gets the number of elements in the list
 * @param list List instance
 * @return Number of elements in list
 */
int list_size(List list);

/**
 * @brief Checks if the list is empty
 * @param list List instance
 * @return true if list is empty, false otherwise
 */
bool list_is_empty(List list);

/**
 * @brief Removes all elements from the list without destroying it
 * @param list List instance
 */
void list_clear(List list);

#endif // LIST_H
