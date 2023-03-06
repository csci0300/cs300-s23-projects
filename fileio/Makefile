
# CC stands for C Compiler.
CC           := gcc
# Which sanitizers should be enabled? These are on by default and you should
# keep it that way
# You should not be making any allocations, so no leak sanitizer (it messes with strace)
SANS_ENABLED ?= -fsanitize=address -fsanitize=undefined
# Flags to be passed to the C compiler.
# -ggdb3 says to embed as much debugging information as possible
# -Wall -Wextra says to enable all standard, and some extra warnings
# -Wshadow explicitly makes shadowing a local variable a warning
# -std=gnu11 says which version of the C language our source program is written in
CFLAGS       += -ggdb3 -Wall -Wextra -Wshadow -Wno-unused-parameter -std=gnu11 $(SANS_ENABLED)

# These programs use your IO library. We will run them to make sure your code is
# working correctly
TEST_PROGRAMS := byte_cat reverse_byte_cat block_cat reverse_block_cat random_block_cat stride_cat rot13


# Which implementation of io300_file do we want to use with our test programs?
# Options are student | naive | stdio
#
# To choose one, you can edit the variable below, or specify its value on the
# command line.
#    $ make -B IMPL=stdio
#
IMPL ?= student

ifeq ($(IMPL), student)
	CFLAGS += -Dfread=DO_NOT_USE_STDIO_fread -Dfwrite=DO_NOT_USE_STDIO_fwrite -Dfputc=DO_NOT_USE_STDIO_fputc -Dfgetc=DO_NOT_USE_STDIO_fgetc
endif

all: $(TEST_PROGRAMS)

%: test_programs/%.c impl/$(IMPL).c
	$(CC) $(CFLAGS) $^ -o $@

check: $(TEST_PROGRAMS)
	./test_scripts/correctness_test.py

perf:
	./test_scripts/performance_test.py

clean:
	rm -f -- $(TEST_PROGRAMS)

testdata:
	dd if=/dev/urandom of=/tmp/testdata bs=1M count=10
	@echo "10MB test file generated in /tmp/testdata"

.PHONY: all clean perf check
