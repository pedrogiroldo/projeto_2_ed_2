#include "extensible_hash_file.h"

#include "unity.h"

#include <stdio.h>
#include <string.h>

typedef struct {
  int id;
  char label[16];
} test_record_t;

static const char *TEST_INDEX_PATH = "/tmp/extensible_hash_file_test.idx";

static test_record_t make_record(int id, const char *label) {
  test_record_t record;

  record.id = id;
  memset(record.label, 0, sizeof(record.label));
  strncpy(record.label, label, sizeof(record.label) - 1u);
  return record;
}

void setUp(void) { remove(TEST_INDEX_PATH); }

void tearDown(void) { remove(TEST_INDEX_PATH); }

void test_ehf_create_returns_handle_for_valid_path(void) {
  extensible_hash_file_t hash =
      ehf_create(TEST_INDEX_PATH, 2u, sizeof(test_record_t));

  TEST_ASSERT_NOT_NULL(hash);
  TEST_ASSERT_TRUE(ehf_is_open(hash));

  ehf_close(hash);
}

void test_ehf_create_rejects_invalid_arguments(void) {
  TEST_ASSERT_NULL(ehf_create(NULL, 2u, sizeof(test_record_t)));
  TEST_ASSERT_NULL(ehf_create(TEST_INDEX_PATH, 0u, sizeof(test_record_t)));
  TEST_ASSERT_NULL(ehf_create(TEST_INDEX_PATH, 2u, 0u));
}

void test_ehf_open_rejects_invalid_path(void) { TEST_ASSERT_NULL(ehf_open(NULL)); }

void test_ehf_open_returns_null_for_missing_file(void) {
  TEST_ASSERT_NULL(ehf_open(TEST_INDEX_PATH));
}

void test_ehf_insert_and_find_existing_key(void) {
  extensible_hash_file_t hash =
      ehf_create(TEST_INDEX_PATH, 2u, sizeof(test_record_t));
  test_record_t expected = make_record(123, "alpha");
  test_record_t found;

  memset(&found, 0, sizeof(found));

  TEST_ASSERT_NOT_NULL(hash);
  TEST_ASSERT_EQUAL_INT(EHF_OK,
                        ehf_insert(hash, "A1", &expected, sizeof(expected)));
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_find(hash, "A1", &found, sizeof(found)));
  TEST_ASSERT_EQUAL_MEMORY(&expected, &found, sizeof(expected));

  ehf_close(hash);
}

void test_ehf_find_returns_not_found_for_missing_key(void) {
  extensible_hash_file_t hash =
      ehf_create(TEST_INDEX_PATH, 2u, sizeof(test_record_t));
  test_record_t found;

  TEST_ASSERT_NOT_NULL(hash);
  TEST_ASSERT_EQUAL_INT(EHF_NOT_FOUND,
                        ehf_find(hash, "ZZ99", &found, sizeof(found)));

  ehf_close(hash);
}

void test_ehf_insert_rejects_duplicate_key(void) {
  extensible_hash_file_t hash =
      ehf_create(TEST_INDEX_PATH, 2u, sizeof(test_record_t));
  test_record_t first = make_record(1000, "first");
  test_record_t second = make_record(2000, "second");

  TEST_ASSERT_NOT_NULL(hash);
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_insert(hash, "USER10", &first, sizeof(first)));
  TEST_ASSERT_EQUAL_INT(EHF_DUPLICATE_KEY,
                        ehf_insert(hash, "USER10", &second, sizeof(second)));

  ehf_close(hash);
}

void test_ehf_remove_existing_key(void) {
  extensible_hash_file_t hash =
      ehf_create(TEST_INDEX_PATH, 2u, sizeof(test_record_t));
  test_record_t record = make_record(1500, "gone");
  test_record_t found;

  TEST_ASSERT_NOT_NULL(hash);
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_insert(hash, "KEY15", &record, sizeof(record)));
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_remove(hash, "KEY15"));
  TEST_ASSERT_EQUAL_INT(EHF_NOT_FOUND,
                        ehf_find(hash, "KEY15", &found, sizeof(found)));

  ehf_close(hash);
}

void test_ehf_remove_missing_key_returns_not_found(void) {
  extensible_hash_file_t hash =
      ehf_create(TEST_INDEX_PATH, 2u, sizeof(test_record_t));

  TEST_ASSERT_NOT_NULL(hash);
  TEST_ASSERT_EQUAL_INT(EHF_NOT_FOUND, ehf_remove(hash, "MISSING44"));

  ehf_close(hash);
}

void test_ehf_persists_data_across_close_and_open(void) {
  extensible_hash_file_t hash =
      ehf_create(TEST_INDEX_PATH, 2u, sizeof(test_record_t));
  test_record_t a = make_record(11, "a");
  test_record_t b = make_record(22, "b");
  test_record_t c = make_record(33, "c");
  test_record_t found;

  TEST_ASSERT_NOT_NULL(hash);
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_insert(hash, "A1", &a, sizeof(a)));
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_insert(hash, "B2", &b, sizeof(b)));
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_insert(hash, "C3", &c, sizeof(c)));
  ehf_close(hash);

  hash = ehf_open(TEST_INDEX_PATH);
  TEST_ASSERT_NOT_NULL(hash);
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_find(hash, "A1", &found, sizeof(found)));
  TEST_ASSERT_EQUAL_MEMORY(&a, &found, sizeof(a));
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_find(hash, "B2", &found, sizeof(found)));
  TEST_ASSERT_EQUAL_MEMORY(&b, &found, sizeof(b));
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_find(hash, "C3", &found, sizeof(found)));
  TEST_ASSERT_EQUAL_MEMORY(&c, &found, sizeof(c));

  ehf_close(hash);
}

void test_ehf_handles_null_arguments_defensively(void) {
  extensible_hash_file_t hash =
      ehf_create(TEST_INDEX_PATH, 2u, sizeof(test_record_t));
  test_record_t record = make_record(1, "one");
  test_record_t found;

  TEST_ASSERT_NOT_NULL(hash);
  TEST_ASSERT_EQUAL_INT(EHF_INVALID_ARGUMENT,
                        ehf_insert(NULL, "A1", &record, sizeof(record)));
  TEST_ASSERT_EQUAL_INT(EHF_INVALID_ARGUMENT,
                        ehf_find(NULL, "A1", &found, sizeof(found)));
  TEST_ASSERT_EQUAL_INT(EHF_INVALID_ARGUMENT,
                        ehf_find(hash, "A1", NULL, sizeof(found)));
  TEST_ASSERT_EQUAL_INT(EHF_INVALID_ARGUMENT, ehf_remove(NULL, "A1"));
  TEST_ASSERT_EQUAL_INT(EHF_INVALID_ARGUMENT,
                        ehf_insert(hash, NULL, &record, sizeof(record)));
  TEST_ASSERT_EQUAL_INT(EHF_INVALID_ARGUMENT,
                        ehf_insert(hash, "", &record, sizeof(record)));
  TEST_ASSERT_EQUAL_INT(EHF_OK,
                        ehf_insert(hash, "abc-123", &record, sizeof(record)));
  TEST_ASSERT_EQUAL_INT(
      EHF_INVALID_ARGUMENT,
      ehf_insert(hash, "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567", &record,
                 sizeof(record)));
  TEST_ASSERT_EQUAL_INT(EHF_INVALID_ARGUMENT,
                        ehf_insert(hash, "A1", NULL, sizeof(record)));
  TEST_ASSERT_EQUAL_INT(EHF_INVALID_ARGUMENT,
                        ehf_insert(hash, "A1", &record, sizeof(record) - 1u));
  TEST_ASSERT_EQUAL_INT(EHF_INVALID_ARGUMENT,
                        ehf_find(hash, "A1", &found, sizeof(found) - 1u));

  ehf_close(hash);
}

void test_ehf_triggers_split_without_breaking_record_storage(void) {
  extensible_hash_file_t hash =
      ehf_create(TEST_INDEX_PATH, 2u, sizeof(test_record_t));
  const char *keys[] = {"A1", "B2", "C3", "D4", "E5",
                        "F6", "G7", "H8", "I9", "J10"};
  test_record_t records[10];
  test_record_t found;
  size_t key;

  TEST_ASSERT_NOT_NULL(hash);

  for (key = 0u; key < 10u; ++key) {
    records[key] = make_record((int)key * 10, keys[key]);
    TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_insert(hash, keys[key], &records[key],
                                            sizeof(records[key])));
  }

  for (key = 0u; key < 10u; ++key) {
    TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_find(hash, keys[key], &found, sizeof(found)));
    TEST_ASSERT_EQUAL_MEMORY(&records[key], &found, sizeof(found));
  }

  ehf_close(hash);
}

void test_ehf_persists_data_after_split_and_reopen(void) {
  extensible_hash_file_t hash =
      ehf_create(TEST_INDEX_PATH, 2u, sizeof(test_record_t));
  const char *keys[] = {"AA1", "BB2", "CC3", "DD4", "EE5", "FF6"};
  test_record_t records[6];
  test_record_t found;
  size_t key;

  TEST_ASSERT_NOT_NULL(hash);

  for (key = 0u; key < 6u; ++key) {
    records[key] = make_record((int)(100u + key), keys[key]);
    TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_insert(hash, keys[key], &records[key],
                                            sizeof(records[key])));
  }

  ehf_close(hash);

  hash = ehf_open(TEST_INDEX_PATH);
  TEST_ASSERT_NOT_NULL(hash);

  for (key = 0u; key < 6u; ++key) {
    TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_find(hash, keys[key], &found, sizeof(found)));
    TEST_ASSERT_EQUAL_MEMORY(&records[key], &found, sizeof(found));
  }

  ehf_close(hash);
}

static void count_visitor(const char *key, const void *record,
                          size_t record_size, void *user_data) {
  (void)key;
  (void)record;
  (void)record_size;
  int *count = (int *)user_data;
  (*count)++;
}

void test_ehf_foreach_iterates_all_records(void) {
  extensible_hash_file_t hash =
      ehf_create(TEST_INDEX_PATH, 4u, sizeof(test_record_t));
  const char *keys[] = {"K1", "K2", "K3", "K4", "K5"};
  test_record_t record;
  int count = 0;
  size_t i;

  TEST_ASSERT_NOT_NULL(hash);
  for (i = 0u; i < 5u; ++i) {
    record = make_record((int)i, keys[i]);
    TEST_ASSERT_EQUAL_INT(EHF_OK,
                          ehf_insert(hash, keys[i], &record, sizeof(record)));
  }

  TEST_ASSERT_EQUAL_INT(EHF_OK,
                        ehf_foreach(hash, count_visitor, sizeof(test_record_t),
                                    &count));
  TEST_ASSERT_EQUAL_INT(5, count);

  ehf_close(hash);
}

void test_ehf_foreach_counts_correctly_after_split(void) {
  extensible_hash_file_t hash =
      ehf_create(TEST_INDEX_PATH, 2u, sizeof(test_record_t));
  const char *keys[] = {"A1", "B2", "C3", "D4", "E5", "F6", "G7", "H8"};
  test_record_t record;
  int count = 0;
  size_t i;

  TEST_ASSERT_NOT_NULL(hash);
  for (i = 0u; i < 8u; ++i) {
    record = make_record((int)i, keys[i]);
    TEST_ASSERT_EQUAL_INT(EHF_OK,
                          ehf_insert(hash, keys[i], &record, sizeof(record)));
  }

  TEST_ASSERT_EQUAL_INT(EHF_OK,
                        ehf_foreach(hash, count_visitor, sizeof(test_record_t),
                                    &count));
  TEST_ASSERT_EQUAL_INT(8, count);

  ehf_close(hash);
}

void test_ehf_foreach_rejects_null_visitor(void) {
  extensible_hash_file_t hash =
      ehf_create(TEST_INDEX_PATH, 2u, sizeof(test_record_t));

  TEST_ASSERT_NOT_NULL(hash);
  TEST_ASSERT_EQUAL_INT(EHF_INVALID_ARGUMENT,
                        ehf_foreach(hash, NULL, sizeof(test_record_t), NULL));
  ehf_close(hash);
}

void test_ehf_dump_creates_readable_file(void) {
  static const char *DUMP_PATH = "/tmp/extensible_hash_file_test.hfd";
  extensible_hash_file_t hash =
      ehf_create(TEST_INDEX_PATH, 2u, sizeof(test_record_t));
  test_record_t record;
  FILE *dump;
  char line[256];

  TEST_ASSERT_NOT_NULL(hash);
  record = make_record(1, "alpha");
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_insert(hash, "A1", &record, sizeof(record)));
  record = make_record(2, "beta");
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_insert(hash, "B2", &record, sizeof(record)));
  record = make_record(3, "gamma");
  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_insert(hash, "C3", &record, sizeof(record)));

  TEST_ASSERT_EQUAL_INT(EHF_OK, ehf_dump(hash, DUMP_PATH));
  ehf_close(hash);

  dump = fopen(DUMP_PATH, "r");
  TEST_ASSERT_NOT_NULL(dump);
  TEST_ASSERT_NOT_NULL(fgets(line, (int)sizeof(line), dump));
  TEST_ASSERT_NOT_NULL(strstr(line, "Extensible Hash File Dump"));
  fclose(dump);
  remove(DUMP_PATH);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_ehf_create_returns_handle_for_valid_path);
  RUN_TEST(test_ehf_create_rejects_invalid_arguments);
  RUN_TEST(test_ehf_open_rejects_invalid_path);
  RUN_TEST(test_ehf_open_returns_null_for_missing_file);
  RUN_TEST(test_ehf_insert_and_find_existing_key);
  RUN_TEST(test_ehf_find_returns_not_found_for_missing_key);
  RUN_TEST(test_ehf_insert_rejects_duplicate_key);
  RUN_TEST(test_ehf_remove_existing_key);
  RUN_TEST(test_ehf_remove_missing_key_returns_not_found);
  RUN_TEST(test_ehf_persists_data_across_close_and_open);
  RUN_TEST(test_ehf_handles_null_arguments_defensively);
  RUN_TEST(test_ehf_triggers_split_without_breaking_record_storage);
  RUN_TEST(test_ehf_persists_data_after_split_and_reopen);
  RUN_TEST(test_ehf_foreach_iterates_all_records);
  RUN_TEST(test_ehf_foreach_counts_correctly_after_split);
  RUN_TEST(test_ehf_foreach_rejects_null_visitor);
  RUN_TEST(test_ehf_dump_creates_readable_file);
  return UNITY_END();
}
