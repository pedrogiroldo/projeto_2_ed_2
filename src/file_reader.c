#include "file_reader.h"
#include "queue.h"
#include "stack.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct FileData {
  const char *filepath;
  const char *filename;
  Queue linesQueue;
  // This stack is used to free the lines of
  // the file after the execution of the commands
  Stack linesStackToFree;
};

struct LinesQueueAndStack {
  Queue linesQueue;
  Stack linesStackToFree;
};

// Private functions
static char *read_line(FILE *file, char *buffer, size_t size);
static struct LinesQueueAndStack *
read_file_to_queue_and_stack(const char *filepath);

// Creates a new FileData instance and reads the file
FileData file_data_create(const char *filepath) {
  struct FileData *file;
  if (filepath == NULL) {
    return NULL;
  }

  file = malloc(sizeof(struct FileData));
  if (file == NULL) {
    printf("Error: Failed to allocate memory for FileData\n");
    return NULL;
  }

  file->filepath = filepath;
  file->filename =
      strrchr(filepath, '/') ? strrchr(filepath, '/') + 1 : filepath;

  struct LinesQueueAndStack *linesQueueAndStack =
      read_file_to_queue_and_stack(filepath);

  if (linesQueueAndStack == NULL || linesQueueAndStack->linesQueue == NULL ||
      linesQueueAndStack->linesStackToFree == NULL) {
    if (linesQueueAndStack != NULL) {
      free(linesQueueAndStack);
    }
    free(file);
    return NULL;
  }

  file->linesQueue = linesQueueAndStack->linesQueue;
  file->linesStackToFree = linesQueueAndStack->linesStackToFree;
  free(linesQueueAndStack);
  return (FileData)file;
}

// Reads the file lines and returns a Queue. This function is private.
static struct LinesQueueAndStack *
read_file_to_queue_and_stack(const char *filepath) {
  struct LinesQueueAndStack *linesQueueAndStack =
      malloc(sizeof(struct LinesQueueAndStack));
  if (linesQueueAndStack == NULL) {
    printf("Error: Failed to allocate memory for LinesQueueAndStack\n");
    return NULL;
  }

  Queue lines = queue_create();
  Stack linesStackToFree = stack_create();
  FILE *file = fopen(filepath, "r");
  if (file == NULL) {
    // Cleanup and report error
    if (lines != NULL) {
      queue_destroy(lines);
    }
    if (linesStackToFree != NULL) {
      stack_destroy(linesStackToFree);
    }
    free(linesQueueAndStack);
    return NULL;
  }

  char buffer[1024];
  while (read_line(file, buffer, sizeof(buffer)) != NULL) {
    char *line = duplicate_string(buffer);
    queue_enqueue(lines, line);
    stack_push(linesStackToFree, line);
  }

  fclose(file);
  // Assign initialized containers to the struct before returning
  linesQueueAndStack->linesQueue = lines;
  linesQueueAndStack->linesStackToFree = linesStackToFree;
  return linesQueueAndStack;
}

// Destroys a FileData instance and frees memory
void file_data_destroy(FileData fileData) {
  if (fileData != NULL) {
    struct FileData *file = (struct FileData *)fileData;
    // This frees the lines of the file
    while (!stack_is_empty(file->linesStackToFree)) {
      void *line = stack_pop(file->linesStackToFree);
      line != NULL ? free(line) : NULL;
    }
    // This destroys the stack of lines to free
    stack_destroy(file->linesStackToFree);
    // This destroys the queue of lines
    queue_destroy(file->linesQueue);
    // This frees the file data
    free(fileData);
  }
}

// Gets the file path
const char *get_file_path(const FileData fileData) {
  struct FileData *file = (struct FileData *)fileData;
  if (file == NULL) {
    return NULL;
  }
  return file->filepath;
}

// Gets the file name
const char *get_file_name(const FileData fileData) {
  struct FileData *file = (struct FileData *)fileData;
  if (file == NULL) {
    return NULL;
  }
  return file->filename;
}

// Gets the file lines queue
Queue get_file_lines_queue(const FileData fileData) {
  struct FileData *file = (struct FileData *)fileData;
  if (file == NULL) {
    return NULL;
  }
  return file->linesQueue;
}

// Reads a line from file using fgets
static char *read_line(FILE *file, char *buffer, size_t size) {
  if (fgets(buffer, size, file) != NULL) {
    // Remove trailing newline(s), supporting LF and CRLF.
    size_t len = strlen(buffer);
    while (len > 0u &&
           (buffer[len - 1u] == '\n' || buffer[len - 1u] == '\r')) {
      buffer[len - 1u] = '\0';
      len--;
    }
    return buffer;
  }
  return NULL;
}
