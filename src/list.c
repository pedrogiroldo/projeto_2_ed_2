#include "list.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
  void *data;
  struct Node *next;
  struct Node *prev;
} Node;

typedef struct {
  Node *head;
  Node *tail;
  int size;
} ListImpl;

List list_create(void) {
  ListImpl *list = malloc(sizeof(ListImpl));
  if (list == NULL) {
    printf("Error: Failed to allocate memory for List\n");
    return NULL;
  }

  list->head = NULL;
  list->tail = NULL;
  list->size = 0;

  return (List)list;
}

void list_destroy(List list) {
  if (list == NULL) {
    return;
  }

  ListImpl *impl = (ListImpl *)list;

  Node *current = impl->head;
  while (current != NULL) {
    Node *next = current->next;
    free(current);
    current = next;
  }

  free(impl);
}

bool list_insert_back(List list, void *data) {
  if (list == NULL) {
    return false;
  }

  ListImpl *impl = (ListImpl *)list;

  Node *new_node = malloc(sizeof(Node));
  if (new_node == NULL) {
    printf("Error: Failed to allocate memory for Node\n");
    return false;
  }

  new_node->data = data;
  new_node->next = NULL;
  new_node->prev = impl->tail;

  if (impl->tail != NULL) {
    impl->tail->next = new_node;
  }

  impl->tail = new_node;

  if (impl->head == NULL) {
    impl->head = new_node;
  }

  impl->size++;
  return true;
}

bool list_insert_front(List list, void *data) {
  if (list == NULL) {
    return false;
  }

  ListImpl *impl = (ListImpl *)list;

  Node *new_node = malloc(sizeof(Node));
  if (new_node == NULL) {
    printf("Error: Failed to allocate memory for Node\n");
    return false;
  }

  new_node->data = data;
  new_node->next = impl->head;
  new_node->prev = NULL;

  if (impl->head != NULL) {
    impl->head->prev = new_node;
  }

  impl->head = new_node;

  if (impl->tail == NULL) {
    impl->tail = new_node;
  }

  impl->size++;
  return true;
}

bool list_remove(List list, void *data) {
  if (list == NULL) {
    return false;
  }

  ListImpl *impl = (ListImpl *)list;
  Node *current = impl->head;

  while (current != NULL) {
    if (current->data == data) {
      // Update previous node's next pointer
      if (current->prev != NULL) {
        current->prev->next = current->next;
      } else {
        // Removing head
        impl->head = current->next;
      }

      // Update next node's prev pointer
      if (current->next != NULL) {
        current->next->prev = current->prev;
      } else {
        // Removing tail
        impl->tail = current->prev;
      }

      free(current);
      impl->size--;
      return true;
    }
    current = current->next;
  }

  return false;
}

void *list_get(List list, int index) {
  if (list == NULL) {
    return NULL;
  }

  ListImpl *impl = (ListImpl *)list;

  if (index < 0 || index >= impl->size) {
    return NULL;
  }

  Node *current = impl->head;
  for (int i = 0; i < index; i++) {
    current = current->next;
  }

  return current->data;
}

void *list_get_first(List list) {
  if (list == NULL) {
    return NULL;
  }

  ListImpl *impl = (ListImpl *)list;

  if (impl->head == NULL) {
    return NULL;
  }

  return impl->head->data;
}

void *list_get_last(List list) {
  if (list == NULL) {
    return NULL;
  }

  ListImpl *impl = (ListImpl *)list;

  if (impl->tail == NULL) {
    return NULL;
  }

  return impl->tail->data;
}

int list_size(List list) {
  if (list == NULL) {
    return 0;
  }

  ListImpl *impl = (ListImpl *)list;
  return impl->size;
}

bool list_is_empty(List list) {
  if (list == NULL) {
    return true;
  }

  ListImpl *impl = (ListImpl *)list;
  return impl->size == 0;
}

void list_clear(List list) {
  if (list == NULL) {
    return;
  }

  ListImpl *impl = (ListImpl *)list;

  Node *current = impl->head;
  while (current != NULL) {
    Node *next = current->next;
    free(current);
    current = next;
  }

  impl->head = NULL;
  impl->tail = NULL;
  impl->size = 0;
}
