PROJ_NAME = ted
LIBS = -lm
TEST_SUPPORT_SRC = ../unity/unity.c

SRC_FILES := $(wildcard *.c)
MAIN_SRC = main.c
APP_SRCS := $(filter-out $(MAIN_SRC),$(SRC_FILES))
OBJETOS  := $(SRC_FILES:.c=.o)

TEST_FILES := $(wildcard ../test/t_*.c)
TEST_BINS  := $(patsubst ../test/t_%.c,../test/%_test,$(TEST_FILES))

CC     = gcc
CFLAGS = -ggdb -O0 -std=c99 -fstack-protector-all -Werror=implicit-function-declaration -I../include
LDFLAGS = -O0

$(PROJ_NAME): $(OBJETOS)
	$(CC) -o $(PROJ_NAME) $(LDFLAGS) $(OBJETOS) $(LIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

# ============================================================================
# Test Targets
# ============================================================================

test: test-build test-run

test-build:
	@for test_src in $(TEST_FILES); do \
		test_bin=$$(echo $$test_src | sed 's|../test/t_\(.*\)\.c|../test/\1_test|'); \
		echo "Building test: $$test_bin"; \
		$(CC) $(CFLAGS) -I../unity -o $$test_bin $$test_src $(APP_SRCS) $(TEST_SUPPORT_SRC) $(LIBS) || exit 1; \
	done

test-run: test-build
	@echo "========================================="
	@echo "Running All Tests"
	@echo "========================================="
	@for test in $(TEST_BINS); do \
		echo ""; \
		echo ">>> Running $$test"; \
		$$test || exit 1; \
	done
	@echo ""
	@echo "========================================="
	@echo "All Tests Passed!"
	@echo "========================================="

test-clean:
	@echo "Cleaning test executables..."
	rm -f $(TEST_BINS)

clean: test-clean
	rm -f $(OBJETOS) $(PROJ_NAME)

debug:
	@echo "SRC_FILES:        $(SRC_FILES)"
	@echo "OBJETOS:          $(OBJETOS)"
	@echo "TEST_FILES:       $(TEST_FILES)"
	@echo "TEST_BINS:        $(TEST_BINS)"
	@echo "TEST_SUPPORT_SRC: $(TEST_SUPPORT_SRC)"

run: $(PROJ_NAME)
	./$(PROJ_NAME) -f ../t2/exemplo.geo -v ../t2/exemplo.via -q ../t2/exemplo.qry -o ../saida

tstall: test

.PHONY: test test-build test-run test-clean clean debug run tstall
