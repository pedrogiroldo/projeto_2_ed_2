/**
 * @file file_reader.h
 * @brief File reading and data management
 *
 * This module provides functions to read .geo and .qry files
 * and manage file data structures.
 */

#ifndef FILE_READER_H
#define FILE_READER_H

#include "queue.h"

/**
 * @brief Opaque pointer type for file data instances
 */
typedef void *FileData;

/**
 * @brief Creates a new FileData instance and reads the file
 *
 * Reads all lines from the specified file and stores them in a queue.
 * The file path and name are also stored for later retrieval.
 *
 * @param filepath Path to the file to be read
 * @return FileData instance or NULL if creation failed
 */
FileData file_data_create(const char *filepath);

/**
 * @brief Destroys a FileData instance and frees all memory
 * @param fileData FileData instance to destroy
 */
void file_data_destroy(FileData fileData);

/**
 * @brief Gets the file path from FileData
 * @param fileData FileData instance
 * @return File path string (do not free)
 */
const char *get_file_path(const FileData fileData);

/**
 * @brief Gets the file name from FileData
 * @param fileData FileData instance
 * @return File name string (do not free)
 */
const char *get_file_name(const FileData fileData);

/**
 * @brief Gets the queue containing all file lines
 * @param fileData FileData instance
 * @return Queue containing file lines
 */
Queue get_file_lines_queue(const FileData fileData);

#endif // FILE_READER_H
