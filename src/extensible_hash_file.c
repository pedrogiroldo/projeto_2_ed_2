#include "extensible_hash_file.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EHF_MAGIC "EHF1"
#define EHF_VERSION 2u
#define EHF_MAX_KEY_LENGTH 32u

typedef struct {
  uint8_t occupied;
  char key[EHF_MAX_KEY_LENGTH + 1u];
  unsigned char *record;
} ehf_entry_t;

typedef struct {
  uint32_t local_depth;
  uint32_t count;
  uint32_t capacity;
  ehf_entry_t *entries;
} ehf_bucket_t;

typedef struct {
  char magic[4];
  uint32_t version;
  uint32_t bucket_capacity;
  uint32_t global_depth;
  uint32_t directory_size;
  uint32_t record_size;
  uint64_t directory_offset;
} ehf_header_t;

typedef struct {
  uint32_t global_depth_before;
  uint32_t global_depth_after;
  uint32_t local_depth_before;
  uint64_t bucket_offset;
} ehf_split_event_t;

typedef struct {
  FILE *file;
  ehf_header_t header;
  bool is_open;
  ehf_split_event_t *split_log;
  uint32_t split_log_size;
  uint32_t split_log_capacity;
} ehf_handle_t;

static bool ehf_write_exact(FILE *file, const void *buffer, size_t size) {
  return fwrite(buffer, 1u, size, file) == size;
}

static bool ehf_read_exact(FILE *file, void *buffer, size_t size) {
  return fread(buffer, 1u, size, file) == size;
}

static bool ehf_seek(FILE *file, uint64_t offset) {
  return fseek(file, (long)offset, SEEK_SET) == 0;
}

static bool ehf_flush(FILE *file) { return fflush(file) == 0; }

static uint64_t ehf_hash_key(const char *key) {
  uint64_t hash = 1469598103934665603ull;

  while (*key != '\0') {
    hash ^= (unsigned char)*key;
    hash *= 1099511628211ull;
    ++key;
  }

  return hash;
}

static bool ehf_is_valid_handle(const ehf_handle_t *handle) {
  return handle != NULL && handle->is_open && handle->file != NULL;
}

static bool ehf_validate_key(const char *key) {
  size_t length;
  size_t index;

  if (key == NULL) {
    return false;
  }

  length = strlen(key);
  if (length == 0u || length > EHF_MAX_KEY_LENGTH) {
    return false;
  }

  for (index = 0u; index < length; ++index) {
    if (!isalnum((unsigned char)key[index]) && key[index] != '.' &&
        key[index] != '-') {
      return false;
    }
  }

  return true;
}

static uint64_t ehf_bucket_record_size(const ehf_handle_t *handle) {
  return sizeof(uint32_t) + sizeof(uint32_t) +
         (uint64_t)handle->header.bucket_capacity *
             (sizeof(uint8_t) + (EHF_MAX_KEY_LENGTH + 1u) +
              handle->header.record_size);
}

static bool ehf_write_header(ehf_handle_t *handle) {
  ehf_header_t *header = &handle->header;

  if (!ehf_seek(handle->file, 0u)) {
    return false;
  }

  if (!ehf_write_exact(handle->file, header->magic, sizeof(header->magic))) {
    return false;
  }

  if (!ehf_write_exact(handle->file, &header->version, sizeof(header->version))) {
    return false;
  }

  if (!ehf_write_exact(handle->file, &header->bucket_capacity,
                       sizeof(header->bucket_capacity))) {
    return false;
  }

  if (!ehf_write_exact(handle->file, &header->global_depth,
                       sizeof(header->global_depth))) {
    return false;
  }

  if (!ehf_write_exact(handle->file, &header->directory_size,
                       sizeof(header->directory_size))) {
    return false;
  }

  if (!ehf_write_exact(handle->file, &header->record_size,
                       sizeof(header->record_size))) {
    return false;
  }

  if (!ehf_write_exact(handle->file, &header->directory_offset,
                       sizeof(header->directory_offset))) {
    return false;
  }

  return ehf_flush(handle->file);
}

static bool ehf_read_header(FILE *file, ehf_header_t *header) {
  if (!ehf_seek(file, 0u)) {
    return false;
  }

  if (!ehf_read_exact(file, header->magic, sizeof(header->magic))) {
    return false;
  }

  if (!ehf_read_exact(file, &header->version, sizeof(header->version))) {
    return false;
  }

  if (!ehf_read_exact(file, &header->bucket_capacity,
                      sizeof(header->bucket_capacity))) {
    return false;
  }

  if (!ehf_read_exact(file, &header->global_depth, sizeof(header->global_depth))) {
    return false;
  }

  if (!ehf_read_exact(file, &header->directory_size,
                      sizeof(header->directory_size))) {
    return false;
  }

  if (!ehf_read_exact(file, &header->record_size, sizeof(header->record_size))) {
    return false;
  }

  if (!ehf_read_exact(file, &header->directory_offset,
                      sizeof(header->directory_offset))) {
    return false;
  }

  return true;
}

static bool ehf_header_is_valid(const ehf_header_t *header) {
  if (memcmp(header->magic, EHF_MAGIC, sizeof(header->magic)) != 0) {
    return false;
  }

  if (header->version != EHF_VERSION || header->bucket_capacity == 0u ||
      header->record_size == 0u) {
    return false;
  }

  if (header->global_depth == 0u || header->global_depth >= 31u) {
    return false;
  }

  if (header->directory_size != (1u << header->global_depth)) {
    return false;
  }

  if (header->directory_offset < sizeof(ehf_header_t)) {
    return false;
  }

  return true;
}

static void ehf_bucket_destroy(ehf_bucket_t *bucket);

static ehf_bucket_t *ehf_bucket_create(uint32_t bucket_capacity,
                                       uint32_t record_size) {
  ehf_bucket_t *bucket = (ehf_bucket_t *)calloc(1u, sizeof(*bucket));
  uint32_t entry_index;

  if (bucket == NULL) {
    return NULL;
  }

  bucket->capacity = bucket_capacity;
  bucket->entries =
      (ehf_entry_t *)calloc(bucket_capacity, sizeof(*bucket->entries));
  if (bucket->entries == NULL) {
    free(bucket);
    return NULL;
  }

  for (entry_index = 0u; entry_index < bucket_capacity; ++entry_index) {
    bucket->entries[entry_index].record =
        (unsigned char *)calloc(1u, (size_t)record_size);
    if (bucket->entries[entry_index].record == NULL) {
      ehf_bucket_destroy(bucket);
      return NULL;
    }
  }

  return bucket;
}

static void ehf_bucket_destroy(ehf_bucket_t *bucket) {
  if (bucket == NULL) {
    return;
  }

  if (bucket->entries != NULL) {
    uint32_t entry_index;

    for (entry_index = 0u; entry_index < bucket->capacity; ++entry_index) {
      free(bucket->entries[entry_index].record);
    }

    free(bucket->entries);
  }

  free(bucket);
}

static bool ehf_write_bucket(const ehf_handle_t *handle, uint64_t offset,
                             const ehf_bucket_t *bucket) {
  uint32_t entry_index;

  if (!ehf_seek(handle->file, offset)) {
    return false;
  }

  if (!ehf_write_exact(handle->file, &bucket->local_depth,
                       sizeof(bucket->local_depth))) {
    return false;
  }

  if (!ehf_write_exact(handle->file, &bucket->count, sizeof(bucket->count))) {
    return false;
  }

  for (entry_index = 0u; entry_index < handle->header.bucket_capacity;
       ++entry_index) {
    const ehf_entry_t *entry = &bucket->entries[entry_index];

    if (!ehf_write_exact(handle->file, &entry->occupied, sizeof(entry->occupied))) {
      return false;
    }

    if (!ehf_write_exact(handle->file, entry->key, sizeof(entry->key))) {
      return false;
    }

    if (!ehf_write_exact(handle->file, entry->record,
                         handle->header.record_size)) {
      return false;
    }
  }

  return ehf_flush(handle->file);
}

static bool ehf_read_bucket(const ehf_handle_t *handle, uint64_t offset,
                            ehf_bucket_t *bucket) {
  uint32_t entry_index;

  if (!ehf_seek(handle->file, offset)) {
    return false;
  }

  if (!ehf_read_exact(handle->file, &bucket->local_depth,
                      sizeof(bucket->local_depth))) {
    return false;
  }

  if (!ehf_read_exact(handle->file, &bucket->count, sizeof(bucket->count))) {
    return false;
  }

  if (bucket->count > handle->header.bucket_capacity) {
    return false;
  }

  for (entry_index = 0u; entry_index < handle->header.bucket_capacity;
       ++entry_index) {
    ehf_entry_t *entry = &bucket->entries[entry_index];

    if (!ehf_read_exact(handle->file, &entry->occupied, sizeof(entry->occupied))) {
      return false;
    }

    if (!ehf_read_exact(handle->file, entry->key, sizeof(entry->key))) {
      return false;
    }

    entry->key[EHF_MAX_KEY_LENGTH] = '\0';

    if (!ehf_read_exact(handle->file, entry->record,
                        handle->header.record_size)) {
      return false;
    }
  }

  return true;
}

static bool ehf_directory_load(const ehf_handle_t *handle, uint64_t **directory_out) {
  uint64_t *directory;

  directory = (uint64_t *)malloc((size_t)handle->header.directory_size * sizeof(*directory));
  if (directory == NULL) {
    return false;
  }

  if (!ehf_seek(handle->file, handle->header.directory_offset) ||
      !ehf_read_exact(handle->file, directory,
                      (size_t)handle->header.directory_size * sizeof(*directory))) {
    free(directory);
    return false;
  }

  *directory_out = directory;
  return true;
}

static bool ehf_directory_store(ehf_handle_t *handle, const uint64_t *directory) {
  if (!ehf_seek(handle->file, handle->header.directory_offset)) {
    return false;
  }

  if (!ehf_write_exact(handle->file, directory,
                       (size_t)handle->header.directory_size * sizeof(*directory))) {
    return false;
  }

  return ehf_flush(handle->file);
}

static bool ehf_directory_append_and_swap(ehf_handle_t *handle, const uint64_t *directory,
                                          uint32_t new_size) {
  uint64_t new_offset;

  if (fseek(handle->file, 0L, SEEK_END) != 0) {
    return false;
  }

  new_offset = (uint64_t)ftell(handle->file);
  if (!ehf_write_exact(handle->file, directory, (size_t)new_size * sizeof(*directory))) {
    return false;
  }

  handle->header.global_depth += 1u;
  handle->header.directory_size = new_size;
  handle->header.directory_offset = new_offset;

  if (!ehf_write_header(handle)) {
    return false;
  }

  return ehf_flush(handle->file);
}

static bool ehf_find_slot(const ehf_handle_t *handle, const ehf_bucket_t *bucket,
                          const char *key, uint32_t *slot_out) {
  uint32_t index;

  for (index = 0u; index < handle->header.bucket_capacity; ++index) {
    if (bucket->entries[index].occupied != 0u &&
        strcmp(bucket->entries[index].key, key) == 0) {
      *slot_out = index;
      return true;
    }
  }

  return false;
}

static bool ehf_find_free_slot(const ehf_handle_t *handle, const ehf_bucket_t *bucket,
                               uint32_t *slot_out) {
  uint32_t index;

  for (index = 0u; index < handle->header.bucket_capacity; ++index) {
    if (bucket->entries[index].occupied == 0u) {
      *slot_out = index;
      return true;
    }
  }

  return false;
}

static void ehf_clear_entry(ehf_entry_t *entry, uint32_t record_size) {
  entry->occupied = 0u;
  memset(entry->key, 0, sizeof(entry->key));
  memset(entry->record, 0, (size_t)record_size);
}

static void ehf_fill_entry(ehf_entry_t *entry, const char *key,
                           const void *record, uint32_t record_size) {
  entry->occupied = 1u;
  memset(entry->key, 0, sizeof(entry->key));
  memcpy(entry->key, key, strlen(key));
  memcpy(entry->record, record, (size_t)record_size);
}

static uint32_t ehf_directory_index(const ehf_handle_t *handle, const char *key) {
  uint64_t hash = ehf_hash_key(key);
  uint64_t mask = (1ull << handle->header.global_depth) - 1ull;

  return (uint32_t)(hash & mask);
}

static uint32_t ehf_directory_index_for_depth(uint64_t hash, uint32_t depth) {
  uint64_t mask = (1ull << depth) - 1ull;
  return (uint32_t)(hash & mask);
}

static bool ehf_append_bucket(ehf_handle_t *handle, const ehf_bucket_t *bucket,
                              uint64_t *offset_out) {
  uint64_t offset;

  if (fseek(handle->file, 0L, SEEK_END) != 0) {
    return false;
  }

  offset = (uint64_t)ftell(handle->file);
  if (!ehf_write_bucket(handle, offset, bucket)) {
    return false;
  }

  *offset_out = offset;
  return true;
}

static int ehf_compare_uint64(const void *a, const void *b) {
  uint64_t x = *(const uint64_t *)a;
  uint64_t y = *(const uint64_t *)b;
  if (x < y) return -1;
  if (x > y) return 1;
  return 0;
}

static void ehf_log_split(ehf_handle_t *handle, uint32_t global_before,
                          uint32_t global_after, uint32_t local_before,
                          uint64_t bucket_offset) {
  ehf_split_event_t *new_log;

  if (handle->split_log_size >= handle->split_log_capacity) {
    uint32_t new_cap =
        handle->split_log_capacity == 0u ? 8u : handle->split_log_capacity * 2u;
    new_log = (ehf_split_event_t *)realloc(
        handle->split_log, (size_t)new_cap * sizeof(*new_log));
    if (new_log == NULL) {
      return;
    }
    handle->split_log = new_log;
    handle->split_log_capacity = new_cap;
  }

  handle->split_log[handle->split_log_size].global_depth_before = global_before;
  handle->split_log[handle->split_log_size].global_depth_after = global_after;
  handle->split_log[handle->split_log_size].local_depth_before = local_before;
  handle->split_log[handle->split_log_size].bucket_offset = bucket_offset;
  handle->split_log_size += 1u;
}

static ehf_status_t ehf_split_bucket(ehf_handle_t *handle, uint64_t **directory_in_out,
                                     uint32_t directory_index, ehf_bucket_t *bucket,
                                     uint64_t bucket_offset) {
  uint64_t *directory = *directory_in_out;
  ehf_bucket_t *new_bucket = NULL;
  ehf_bucket_t *redistributed_bucket = NULL;
  uint64_t new_bucket_offset = 0u;
  uint32_t old_local_depth;
  uint32_t new_local_depth;
  uint32_t entry_index;
  uint32_t global_before;

  global_before = handle->header.global_depth;
  old_local_depth = bucket->local_depth;
  new_local_depth = old_local_depth + 1u;

  if (old_local_depth == handle->header.global_depth) {
    uint64_t *expanded_directory;
    uint32_t old_size = handle->header.directory_size;
    uint32_t new_size = old_size * 2u;
    uint32_t index;

    expanded_directory = (uint64_t *)malloc((size_t)new_size * sizeof(*expanded_directory));
    if (expanded_directory == NULL) {
      return EHF_IO_ERROR;
    }

    for (index = 0u; index < old_size; ++index) {
      expanded_directory[index] = directory[index];
      expanded_directory[index + old_size] = directory[index];
    }

    if (!ehf_directory_append_and_swap(handle, expanded_directory, new_size)) {
      free(expanded_directory);
      return EHF_IO_ERROR;
    }

    free(directory);
    directory = expanded_directory;
    *directory_in_out = directory;
  }

  redistributed_bucket =
      ehf_bucket_create(handle->header.bucket_capacity, handle->header.record_size);
  new_bucket =
      ehf_bucket_create(handle->header.bucket_capacity, handle->header.record_size);
  if (redistributed_bucket == NULL || new_bucket == NULL) {
    ehf_bucket_destroy(redistributed_bucket);
    ehf_bucket_destroy(new_bucket);
    return EHF_IO_ERROR;
  }

  redistributed_bucket->local_depth = new_local_depth;
  new_bucket->local_depth = new_local_depth;

  for (entry_index = 0u; entry_index < handle->header.bucket_capacity; ++entry_index) {
    const ehf_entry_t *entry = &bucket->entries[entry_index];
    ehf_bucket_t *target_bucket;
    uint32_t slot;
    uint32_t index;

    if (entry->occupied == 0u) {
      continue;
    }

    index = ehf_directory_index_for_depth(ehf_hash_key(entry->key), new_local_depth);
    target_bucket =
        ((index & (1u << old_local_depth)) == 0u) ? redistributed_bucket : new_bucket;

    if (!ehf_find_free_slot(handle, target_bucket, &slot)) {
      ehf_bucket_destroy(redistributed_bucket);
      ehf_bucket_destroy(new_bucket);
      return EHF_CORRUPTED_FILE;
    }

    ehf_fill_entry(&target_bucket->entries[slot], entry->key, entry->record,
                   handle->header.record_size);
    target_bucket->count += 1u;
  }

  if (!ehf_append_bucket(handle, new_bucket, &new_bucket_offset)) {
    ehf_bucket_destroy(redistributed_bucket);
    ehf_bucket_destroy(new_bucket);
    return EHF_IO_ERROR;
  }

  for (entry_index = 0u; entry_index < handle->header.directory_size; ++entry_index) {
    if (directory[entry_index] == bucket_offset) {
      if ((entry_index & (1u << old_local_depth)) != 0u) {
        directory[entry_index] = new_bucket_offset;
      }
    }
  }

  if (!ehf_write_bucket(handle, bucket_offset, redistributed_bucket) ||
      !ehf_directory_store(handle, directory)) {
    ehf_bucket_destroy(redistributed_bucket);
    ehf_bucket_destroy(new_bucket);
    return EHF_IO_ERROR;
  }

  bucket->local_depth = redistributed_bucket->local_depth;
  bucket->count = redistributed_bucket->count;
  for (entry_index = 0u; entry_index < handle->header.bucket_capacity;
       ++entry_index) {
    if (redistributed_bucket->entries[entry_index].occupied != 0u) {
      ehf_fill_entry(&bucket->entries[entry_index],
                     redistributed_bucket->entries[entry_index].key,
                     redistributed_bucket->entries[entry_index].record,
                     handle->header.record_size);
    } else {
      ehf_clear_entry(&bucket->entries[entry_index], handle->header.record_size);
    }
  }

  (void)directory_index;
  ehf_log_split(handle, global_before, handle->header.global_depth,
                old_local_depth, bucket_offset);
  ehf_bucket_destroy(redistributed_bucket);
  ehf_bucket_destroy(new_bucket);
  return EHF_OK;
}

static ehf_status_t ehf_insert_internal(ehf_handle_t *handle, const char *key,
                                        const void *record) {
  uint64_t *directory = NULL;
  ehf_bucket_t *bucket = NULL;
  ehf_status_t status = EHF_IO_ERROR;

  if (!ehf_directory_load(handle, &directory)) {
    return EHF_IO_ERROR;
  }

  bucket = ehf_bucket_create(handle->header.bucket_capacity,
                             handle->header.record_size);
  if (bucket == NULL) {
    free(directory);
    return EHF_IO_ERROR;
  }

  for (;;) {
    uint32_t directory_index = ehf_directory_index(handle, key);
    uint64_t bucket_offset = directory[directory_index];
    uint32_t slot;

    if (!ehf_read_bucket(handle, bucket_offset, bucket)) {
      status = EHF_CORRUPTED_FILE;
      break;
    }

    if (ehf_find_slot(handle, bucket, key, &slot)) {
      status = EHF_DUPLICATE_KEY;
      break;
    }

    if (ehf_find_free_slot(handle, bucket, &slot)) {
      ehf_fill_entry(&bucket->entries[slot], key, record,
                     handle->header.record_size);
      bucket->count += 1u;

      status = ehf_write_bucket(handle, bucket_offset, bucket) ? EHF_OK : EHF_IO_ERROR;
      break;
    }

    status = ehf_split_bucket(handle, &directory, directory_index, bucket, bucket_offset);
    if (status != EHF_OK) {
      break;
    }
  }

  ehf_bucket_destroy(bucket);
  free(directory);
  return status;
}

extensible_hash_file_t ehf_create(const char *index_path, uint32_t bucket_capacity,
                                  size_t record_size) {
  ehf_handle_t *handle;
  uint64_t directory[2];
  ehf_bucket_t *bucket0 = NULL;
  ehf_bucket_t *bucket1 = NULL;
  uint64_t bucket0_offset;
  uint64_t bucket1_offset;

  if (index_path == NULL || bucket_capacity == 0u || record_size == 0u ||
      record_size > UINT32_MAX) {
    return NULL;
  }

  handle = (ehf_handle_t *)calloc(1u, sizeof(*handle));
  if (handle == NULL) {
    return NULL;
  }

  handle->file = fopen(index_path, "w+b");
  if (handle->file == NULL) {
    free(handle);
    return NULL;
  }

  memcpy(handle->header.magic, EHF_MAGIC, sizeof(handle->header.magic));
  handle->header.version = EHF_VERSION;
  handle->header.bucket_capacity = bucket_capacity;
  handle->header.global_depth = 1u;
  handle->header.directory_size = 2u;
  handle->header.record_size = (uint32_t)record_size;
  handle->header.directory_offset = sizeof(ehf_header_t);
  handle->is_open = true;

  bucket0 = ehf_bucket_create(bucket_capacity, handle->header.record_size);
  bucket1 = ehf_bucket_create(bucket_capacity, handle->header.record_size);
  if (bucket0 == NULL || bucket1 == NULL) {
    ehf_bucket_destroy(bucket0);
    ehf_bucket_destroy(bucket1);
    ehf_close(handle);
    return NULL;
  }

  bucket0->local_depth = 1u;
  bucket1->local_depth = 1u;

  bucket0_offset =
      handle->header.directory_offset + ((uint64_t)handle->header.directory_size * sizeof(uint64_t));
  bucket1_offset = bucket0_offset + ehf_bucket_record_size(handle);
  directory[0] = bucket0_offset;
  directory[1] = bucket1_offset;

  if (!ehf_write_header(handle) || !ehf_directory_store(handle, directory) ||
      !ehf_write_bucket(handle, bucket0_offset, bucket0) ||
      !ehf_write_bucket(handle, bucket1_offset, bucket1)) {
    ehf_bucket_destroy(bucket0);
    ehf_bucket_destroy(bucket1);
    ehf_close(handle);
    return NULL;
  }

  ehf_bucket_destroy(bucket0);
  ehf_bucket_destroy(bucket1);
  return handle;
}

extensible_hash_file_t ehf_open(const char *index_path) {
  ehf_handle_t *handle;

  if (index_path == NULL) {
    return NULL;
  }

  handle = (ehf_handle_t *)calloc(1u, sizeof(*handle));
  if (handle == NULL) {
    return NULL;
  }

  handle->file = fopen(index_path, "r+b");
  if (handle->file == NULL) {
    free(handle);
    return NULL;
  }

  if (!ehf_read_header(handle->file, &handle->header) ||
      !ehf_header_is_valid(&handle->header)) {
    fclose(handle->file);
    free(handle);
    return NULL;
  }

  handle->is_open = true;
  return handle;
}

void ehf_close(extensible_hash_file_t hash) {
  ehf_handle_t *handle = (ehf_handle_t *)hash;

  if (handle == NULL) {
    return;
  }

  if (handle->file != NULL) {
    fclose(handle->file);
    handle->file = NULL;
  }

  free(handle->split_log);
  handle->is_open = false;
  free(handle);
}

ehf_status_t ehf_insert(extensible_hash_file_t hash, const char *key,
                        const void *record, size_t record_size) {
  ehf_handle_t *handle = (ehf_handle_t *)hash;

  if (!ehf_is_valid_handle(handle) || !ehf_validate_key(key) ||
      record == NULL || record_size != handle->header.record_size) {
    return EHF_INVALID_ARGUMENT;
  }

  return ehf_insert_internal(handle, key, record);
}

ehf_status_t ehf_find(extensible_hash_file_t hash, const char *key,
                      void *out_record, size_t record_size) {
  ehf_handle_t *handle = (ehf_handle_t *)hash;
  uint64_t *directory = NULL;
  ehf_bucket_t *bucket = NULL;
  ehf_status_t status = EHF_NOT_FOUND;
  uint32_t slot;

  if (!ehf_is_valid_handle(handle) || !ehf_validate_key(key) ||
      out_record == NULL || record_size != handle->header.record_size) {
    return EHF_INVALID_ARGUMENT;
  }

  if (!ehf_directory_load(handle, &directory)) {
    return EHF_IO_ERROR;
  }

  bucket = ehf_bucket_create(handle->header.bucket_capacity,
                             handle->header.record_size);
  if (bucket == NULL) {
    free(directory);
    return EHF_IO_ERROR;
  }

  if (!ehf_read_bucket(handle, directory[ehf_directory_index(handle, key)], bucket)) {
    status = EHF_CORRUPTED_FILE;
  } else if (ehf_find_slot(handle, bucket, key, &slot)) {
    memcpy(out_record, bucket->entries[slot].record, record_size);
    status = EHF_OK;
  }

  ehf_bucket_destroy(bucket);
  free(directory);
  return status;
}

ehf_status_t ehf_remove(extensible_hash_file_t hash, const char *key) {
  ehf_handle_t *handle = (ehf_handle_t *)hash;
  uint64_t *directory = NULL;
  ehf_bucket_t *bucket = NULL;
  ehf_status_t status = EHF_NOT_FOUND;
  uint32_t slot;
  uint32_t directory_index;
  uint64_t bucket_offset;

  if (!ehf_is_valid_handle(handle) || !ehf_validate_key(key)) {
    return EHF_INVALID_ARGUMENT;
  }

  if (!ehf_directory_load(handle, &directory)) {
    return EHF_IO_ERROR;
  }

  bucket = ehf_bucket_create(handle->header.bucket_capacity,
                             handle->header.record_size);
  if (bucket == NULL) {
    free(directory);
    return EHF_IO_ERROR;
  }

  directory_index = ehf_directory_index(handle, key);
  bucket_offset = directory[directory_index];
  if (!ehf_read_bucket(handle, bucket_offset, bucket)) {
    status = EHF_CORRUPTED_FILE;
  } else if (ehf_find_slot(handle, bucket, key, &slot)) {
    ehf_clear_entry(&bucket->entries[slot], handle->header.record_size);
    bucket->count -= 1u;
    status = ehf_write_bucket(handle, bucket_offset, bucket) ? EHF_OK : EHF_IO_ERROR;
  }

  ehf_bucket_destroy(bucket);
  free(directory);
  return status;
}

bool ehf_is_open(extensible_hash_file_t hash) {
  const ehf_handle_t *handle = (const ehf_handle_t *)hash;

  return ehf_is_valid_handle(handle);
}

ehf_status_t ehf_foreach(extensible_hash_file_t hash, ehf_visitor_fn visitor,
                         size_t record_size, void *user_data) {
  ehf_handle_t *handle = (ehf_handle_t *)hash;
  uint64_t *directory = NULL;
  uint64_t *sorted_offsets = NULL;
  ehf_bucket_t *bucket = NULL;
  uint64_t prev_offset;
  uint32_t i;
  uint32_t entry_i;
  ehf_status_t status = EHF_OK;

  if (!ehf_is_valid_handle(handle) || visitor == NULL ||
      record_size != handle->header.record_size) {
    return EHF_INVALID_ARGUMENT;
  }

  if (!ehf_directory_load(handle, &directory)) {
    return EHF_IO_ERROR;
  }

  sorted_offsets = (uint64_t *)malloc(
      (size_t)handle->header.directory_size * sizeof(*sorted_offsets));
  if (sorted_offsets == NULL) {
    free(directory);
    return EHF_IO_ERROR;
  }
  memcpy(sorted_offsets, directory,
         (size_t)handle->header.directory_size * sizeof(*sorted_offsets));
  qsort(sorted_offsets, (size_t)handle->header.directory_size,
        sizeof(*sorted_offsets), ehf_compare_uint64);

  bucket = ehf_bucket_create(handle->header.bucket_capacity,
                             handle->header.record_size);
  if (bucket == NULL) {
    free(sorted_offsets);
    free(directory);
    return EHF_IO_ERROR;
  }

  prev_offset = (uint64_t)-1;
  for (i = 0u; i < handle->header.directory_size; ++i) {
    uint64_t offset = sorted_offsets[i];

    if (offset == prev_offset) {
      continue;
    }
    prev_offset = offset;

    if (!ehf_read_bucket(handle, offset, bucket)) {
      status = EHF_CORRUPTED_FILE;
      break;
    }

    for (entry_i = 0u; entry_i < handle->header.bucket_capacity; ++entry_i) {
      if (bucket->entries[entry_i].occupied != 0u) {
        visitor(bucket->entries[entry_i].key,
                bucket->entries[entry_i].record,
                record_size, user_data);
      }
    }
  }

  ehf_bucket_destroy(bucket);
  free(sorted_offsets);
  free(directory);
  return status;
}

ehf_status_t ehf_dump(extensible_hash_file_t hash, const char *output_path) {
  ehf_handle_t *handle = (ehf_handle_t *)hash;
  FILE *out;
  uint64_t *directory = NULL;
  uint64_t *sorted_offsets = NULL;
  ehf_bucket_t *bucket = NULL;
  uint64_t prev_offset;
  uint32_t i;
  uint32_t entry_i;
  uint32_t bucket_num;
  ehf_status_t status = EHF_OK;

  if (!ehf_is_valid_handle(handle) || output_path == NULL) {
    return EHF_INVALID_ARGUMENT;
  }

  out = fopen(output_path, "w");
  if (out == NULL) {
    return EHF_IO_ERROR;
  }

  fprintf(out, "=== Extensible Hash File Dump ===\n");
  fprintf(out, "Global Depth   : %u\n", handle->header.global_depth);
  fprintf(out, "Directory Size : %u\n", handle->header.directory_size);
  fprintf(out, "Bucket Capacity: %u\n", handle->header.bucket_capacity);
  fprintf(out, "Record Size    : %u bytes\n\n", handle->header.record_size);

  if (!ehf_directory_load(handle, &directory)) {
    fclose(out);
    return EHF_IO_ERROR;
  }

  fprintf(out, "=== Directory ===\n");
  for (i = 0u; i < handle->header.directory_size; ++i) {
    fprintf(out, "  [%u] -> offset %llu\n", i,
            (unsigned long long)directory[i]);
  }
  fprintf(out, "\n");

  sorted_offsets = (uint64_t *)malloc(
      (size_t)handle->header.directory_size * sizeof(*sorted_offsets));
  if (sorted_offsets == NULL) {
    free(directory);
    fclose(out);
    return EHF_IO_ERROR;
  }
  memcpy(sorted_offsets, directory,
         (size_t)handle->header.directory_size * sizeof(*sorted_offsets));
  qsort(sorted_offsets, (size_t)handle->header.directory_size,
        sizeof(*sorted_offsets), ehf_compare_uint64);

  bucket = ehf_bucket_create(handle->header.bucket_capacity,
                             handle->header.record_size);
  if (bucket == NULL) {
    free(sorted_offsets);
    free(directory);
    fclose(out);
    return EHF_IO_ERROR;
  }

  fprintf(out, "=== Buckets ===\n");
  prev_offset = (uint64_t)-1;
  bucket_num = 0u;
  for (i = 0u; i < handle->header.directory_size; ++i) {
    uint64_t offset = sorted_offsets[i];

    if (offset == prev_offset) {
      continue;
    }
    prev_offset = offset;

    if (!ehf_read_bucket(handle, offset, bucket)) {
      status = EHF_CORRUPTED_FILE;
      break;
    }

    fprintf(out,
            "Bucket #%u (offset=%llu, local_depth=%u, count=%u/%u)\n",
            bucket_num++, (unsigned long long)offset,
            bucket->local_depth, bucket->count,
            handle->header.bucket_capacity);

    for (entry_i = 0u; entry_i < handle->header.bucket_capacity; ++entry_i) {
      if (bucket->entries[entry_i].occupied != 0u) {
        fprintf(out, "  [%u] key=\"%s\"\n", entry_i,
                bucket->entries[entry_i].key);
      } else {
        fprintf(out, "  [%u] (empty)\n", entry_i);
      }
    }
  }

  fprintf(out, "\n=== Split Log (%u event(s)) ===\n",
          handle->split_log_size);
  for (i = 0u; i < handle->split_log_size; ++i) {
    const ehf_split_event_t *ev = &handle->split_log[i];
    fprintf(out,
            "  Split #%u: global_depth %u -> %u, local_depth_before=%u,"
            " bucket_offset=%llu\n",
            i, ev->global_depth_before, ev->global_depth_after,
            ev->local_depth_before,
            (unsigned long long)ev->bucket_offset);
  }

  ehf_bucket_destroy(bucket);
  free(sorted_offsets);
  free(directory);
  fclose(out);
  return status;
}
