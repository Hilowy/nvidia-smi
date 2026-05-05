CC      := gcc
CFLAGS  := -std=c11 -O2 -Wall -Wextra -Werror -pedantic
CPPFLAGS:= -I./main/library -I./main/decode -I./main/exceptions -I./main/support

SRC_DECODE := main/decoder/decode_workload.c
SRC_POLICY := main/exceptions/policy_rules.c
SRC_IDS    := main/support/hw_ids.c

TEST_BIN := tests/test_nvshim

.PHONY: all test lint clean

all: $(TEST_BIN)

$(TEST_BIN): tests/test_nvshim.c $(SRC_DECODE) $(SRC_POLICY) $(SRC_IDS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

lint:
	$(CC) $(CFLAGS) $(CPPFLAGS) -fsyntax-only tests/test_nvshim.c $(SRC_DECODE) $(SRC_POLICY) $(SRC_IDS)

clean:
	rm -f $(TEST_BIN)
