#include "file_reader.h"

#include "unity.h"

#include <stdio.h>
#include <string.h>

static const char *FILE_PATH = "/tmp/file_reader_test.geo";

static void write_file(const char *contents) {
    FILE *f = fopen(FILE_PATH, "w");
    if (f != NULL) {
        fputs(contents, f);
        fclose(f);
    }
}

void setUp(void) { remove(FILE_PATH); }
void tearDown(void) { remove(FILE_PATH); }

void test_file_reader_carrega_linhas_sem_newline(void) {
    FileData fd;
    Queue lines;
    char *line;

    write_file("cq 1 red black\r\nq cep1 0 0 10 10\n");
    fd = file_data_create(FILE_PATH);
    TEST_ASSERT_NOT_NULL(fd);
    TEST_ASSERT_EQUAL_STRING(FILE_PATH, get_file_path(fd));
    TEST_ASSERT_EQUAL_STRING("file_reader_test.geo", get_file_name(fd));

    lines = get_file_lines_queue(fd);
    TEST_ASSERT_EQUAL_INT(2, queue_size(lines));
    line = (char *)queue_dequeue(lines);
    TEST_ASSERT_EQUAL_STRING("cq 1 red black", line);
    line = (char *)queue_dequeue(lines);
    TEST_ASSERT_EQUAL_STRING("q cep1 0 0 10 10", line);

    file_data_destroy(fd);
}

void test_file_reader_rejeita_arquivo_inexistente(void) {
    TEST_ASSERT_NULL(file_data_create("/tmp/file_reader_nao_existe.geo"));
}

void test_file_reader_rejeita_null_com_seguranca(void) {
    TEST_ASSERT_NULL(file_data_create(NULL));
    TEST_ASSERT_NULL(get_file_path(NULL));
    TEST_ASSERT_NULL(get_file_name(NULL));
    TEST_ASSERT_NULL(get_file_lines_queue(NULL));
    file_data_destroy(NULL);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_file_reader_carrega_linhas_sem_newline);
    RUN_TEST(test_file_reader_rejeita_arquivo_inexistente);
    RUN_TEST(test_file_reader_rejeita_null_com_seguranca);
    return UNITY_END();
}
