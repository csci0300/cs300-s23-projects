CC = clang++-12 -std=c++20

PROJECT_SRC = ../
# TODO: remove the -Wno-... flags later, just kinda annoying while compiling lol
CPPFLAGS += -Wall -Wextra -pthread -I$(PROJECT_SRC) -g -Wno-unused-parameter

ASANFLAG = -fsanitize=address
TSANFLAG = -fsanitize=thread

# Add sanitizers if provided
ifeq ($(ASAN),1)
CPPFLAGS += $(ASANFLAG)
endif
ifeq ($(TSAN),1)
CPPFLAGS += $(TSANFLAG)
endif


CLIENT_SRC = ../client
COMMON_SRC = ../common
KVSTORE_SRC = ../kvstore
NET_SRC = ../net
REPL_SRC = ../repl
SERVER_SRC = ../server
SHARDMASTER_SRC = ../shardmaster

CLIENT_OBJ = ./client_dir
COMMON_OBJ = ./common_dir
KVSTORE_OBJ = ./kvstore_dir
NET_OBJ = ./net_dir
REPL_OBJ = ./repl_dir
SERVER_OBJ = ./server_dir
SHARDMASTER_OBJ = ./shardmaster_dir

CLIENT_SRCS = $(wildcard $(CLIENT_SRC)/*.cpp)
COMMON_SRCS = $(wildcard $(COMMON_SRC)/*.cpp)
KVSTORE_SRCS = $(wildcard $(KVSTORE_SRC)/*.cpp)
NET_SRCS = $(wildcard $(NET_SRC)/*.cpp)
REPL_SRCS = $(wildcard $(REPL_SRC)/*.cpp)
SERVER_SRCS = $(wildcard $(SERVER_SRC)/*.cpp)
SHARDMASTER_SRCS = $(wildcard $(SHARDMASTER_SRC)/*.cpp)

CLIENT_OBJS = $(patsubst $(CLIENT_SRC)/%.cpp,$(CLIENT_OBJ)/%.o,$(CLIENT_SRCS))
COMMON_OBJS = $(patsubst $(COMMON_SRC)/%.cpp,$(COMMON_OBJ)/%.o,$(COMMON_SRCS))
KVSTORE_OBJS = $(patsubst $(KVSTORE_SRC)/%.cpp,$(KVSTORE_OBJ)/%.o,$(KVSTORE_SRCS))
NET_OBJS = $(patsubst $(NET_SRC)/%.cpp,$(NET_OBJ)/%.o,$(NET_SRCS))
REPL_OBJS = $(patsubst $(REPL_SRC)/%.cpp,$(REPL_OBJ)/%.o,$(REPL_SRCS))
SERVER_OBJS = $(patsubst $(SERVER_SRC)/%.cpp,$(SERVER_OBJ)/%.o,$(SERVER_SRCS))
SHARDMASTER_OBJS = $(patsubst $(SHARDMASTER_SRC)/%.cpp,$(SHARDMASTER_OBJ)/%.o,$(SHARDMASTER_SRCS))

# ====== Testing stuff
TEST_UTILS_OBJ = ./test_utils
TEST_UTILS_SRC = ../test_utils
TEST_UTILS_SRCS = $(wildcard $(TEST_UTILS_SRC)/*.cpp)
TEST_UTILS_OBJS = $(patsubst $(TEST_UTILS_SRC)/%.cpp,$(TEST_UTILS_OBJ)/%.o,$(TEST_UTILS_SRCS))

QUEUE_TESTS_OBJ = ./queue_tests
QUEUE_TESTS_SRC = ../tests/queue_tests
QUEUE_TESTS_SRCS = $(wildcard $(QUEUE_TESTS_SRC)/*.cpp)
QUEUE_TESTS_OBJS = $(patsubst $(QUEUE_TESTS_SRC)/%.cpp,$(QUEUE_TESTS_OBJ)/%.o,$(QUEUE_TESTS_SRCS))

KVSTORE_SEQUENTIAL_TESTS_OBJ = ./kvstore_sequential_tests
KVSTORE_SEQUENTIAL_TESTS_SRC = ../tests/kvstore_sequential_tests
KVSTORE_SEQUENTIAL_TESTS_SRCS = $(wildcard $(KVSTORE_SEQUENTIAL_TESTS_SRC)/*.cpp)
KVSTORE_SEQUENTIAL_TESTS_OBJS = $(patsubst $(KVSTORE_SEQUENTIAL_TESTS_SRC)/%.cpp,$(KVSTORE_SEQUENTIAL_TESTS_OBJ)/%.o,$(KVSTORE_SEQUENTIAL_TESTS_SRCS))

KVSTORE_PARALLEL_TESTS_OBJ = ./kvstore_parallel_tests
KVSTORE_PARALLEL_TESTS_SRC = ../tests/kvstore_parallel_tests
KVSTORE_PARALLEL_TESTS_SRCS = $(wildcard $(KVSTORE_PARALLEL_TESTS_SRC)/*.cpp)
KVSTORE_PARALLEL_TESTS_OBJS = $(patsubst $(KVSTORE_PARALLEL_TESTS_SRC)/%.cpp,$(KVSTORE_PARALLEL_TESTS_OBJ)/%.o,$(KVSTORE_PARALLEL_TESTS_SRCS))

KVSTORE_PERFORMANCE_TESTS_OBJ = ./kvstore_performance_tests
KVSTORE_PERFORMANCE_TESTS_SRC = ../tests/kvstore_performance_tests
KVSTORE_PERFORMANCE_TESTS_SRCS = $(wildcard $(KVSTORE_PERFORMANCE_TESTS_SRC)/*.cpp)
KVSTORE_PERFORMANCE_TESTS_OBJS = $(patsubst $(KVSTORE_PERFORMANCE_TESTS_SRC)/%.cpp,$(KVSTORE_PERFORMANCE_TESTS_OBJ)/%.o,$(KVSTORE_PERFORMANCE_TESTS_SRCS))

KVSTORE_INTEGRATION_TESTS_OBJ = ./kvstore_integration_tests
KVSTORE_INTEGRATION_TESTS_SRC = ../tests/kvstore_integration_tests
KVSTORE_INTEGRATION_TESTS_SRCS = $(wildcard $(KVSTORE_INTEGRATION_TESTS_SRC)/*.cpp)
KVSTORE_INTEGRATION_TESTS_OBJS = $(patsubst $(KVSTORE_INTEGRATION_TESTS_SRC)/%.cpp,$(KVSTORE_INTEGRATION_TESTS_OBJ)/%.o,$(KVSTORE_INTEGRATION_TESTS_SRCS))

SHARDMASTER_TESTS_OBJ = ./shardmaster_tests
SHARDMASTER_TESTS_SRC = ../tests/shardmaster_tests
SHARDMASTER_TESTS_SRCS = $(wildcard $(SHARDMASTER_TESTS_SRC)/*.cpp)
SHARDMASTER_TESTS_OBJS = $(patsubst $(SHARDMASTER_TESTS_SRC)/%.cpp,$(SHARDMASTER_TESTS_OBJ)/%.o,$(SHARDMASTER_TESTS_SRCS))

SHARDKV_TESTS_OBJ = ./shardkv_tests
SHARDKV_TESTS_SRC = ../tests/shardkv_tests
SHARDKV_TESTS_SRCS = $(wildcard $(SHARDKV_TESTS_SRC)/*.cpp)
SHARDKV_TESTS_OBJS = $(patsubst $(SHARDKV_TESTS_SRC)/%.cpp,$(SHARDKV_TESTS_OBJ)/%.o,$(SHARDKV_TESTS_SRCS))

# TODO: narrow this
TEST_DEPENDENCIES = $(COMMON_OBJS) $(NET_OBJS) $(KVSTORE_OBJS) $(SERVER_OBJS) $(SHARDMASTER_OBJS) $(TEST_UTILS_OBJS)

# All objects, for cleanup
OBJS = $(CLIENT_OBJS) $(COMMON_OBJS) $(KVSTORE_OBJS) $(NET_OBJS) $(REPL_OBJS) $(SERVER_OBJS) $(SHARDMASTER_OBJS)
OBJS += $(TEST_UTILS_OBJS) $(QUEUE_TESTS_OBJS) $(KVSTORE_SEQUENTIAL_TESTS_OBJS) $(KVSTORE_PARALLEL_TESTS_OBJS) $(SHARDMASTER_TESTS_OBJS) $(SHARDKV_TESTS_OBJS)

# make all directories
OBJ_DIRS = $(CLIENT_OBJ) $(COMMON_OBJ) $(KVSTORE_OBJ) $(NET_OBJ) $(REPL_OBJ) $(SERVER_OBJ) $(SHARDMASTER_OBJ)
OBJ_DIRS += $(TEST_UTILS_OBJ) $(QUEUE_TESTS_OBJ) $(KVSTORE_SEQUENTIAL_TESTS_OBJ) $(KVSTORE_PARALLEL_TESTS_OBJ) $(SHARDMASTER_TESTS_OBJ) $(SHARDKV_TESTS_OBJ)

EXEC_DIR = ../cmd
EXECS = simple_client client server shardmaster
# Kinda scuffed, but don't want to include ./test_utils/
TESTS = $(filter-out ./test_utils,$(wildcard ./test_*))

all: compiler_exists $(OBJ_DIRS) $(EXECS)

# Make object directories if they don't exist
$(OBJ_DIRS):
	mkdir -p $@

compiler_exists:
	@clang++-12 --version || (echo "You need to install a new Clang version for this project; please see the handout for the commands!" && exit 1)
	@g++-10 --version || (echo "You need to install a new GCC version for this project; please see the handout for the commands!" && exit 1)

# TODO: what are the LDFLAGS used for in DS's makefile? specifically, -Wl,--as-needed and -ldl
simple_client: $(COMMON_OBJS) $(NET_OBJS) $(REPL_OBJS) $(CLIENT_OBJS) $(EXEC_DIR)/client.cpp
	$(CC) $(CPPFLAGS) -DSIMPLE_CLIENT $^ -o $@

client: $(COMMON_OBJS) $(NET_OBJS) $(REPL_OBJS) $(CLIENT_OBJS) $(EXEC_DIR)/client.cpp
	$(CC) $(CPPFLAGS) $^ -o $@

server: $(COMMON_OBJS) $(NET_OBJS) $(REPL_OBJS) $(KVSTORE_OBJS) $(SERVER_OBJS) $(EXEC_DIR)/server.cpp
	$(CC) $(CPPFLAGS) $^ -o $@

shardmaster: $(COMMON_OBJS) $(NET_OBJS) $(REPL_OBJS) $(SHARDMASTER_OBJS) $(EXEC_DIR)/shardmaster.cpp
	$(CC) $(CPPFLAGS) $^ -o $@

clean:
	rm -f $(EXECS) $(OBJS) $(TESTS)

# Formatting entire directory: https://stackoverflow.com/a/36046965
format:
	find ../ -iname '*.hpp' -o -iname '*.cpp' | xargs clang-format -i -style='{BasedOnStyle: google, DerivePointerAlignment: false, PointerAlignment: Left, AllowShortFunctionsOnASingleLine: None}'

# See here for more: https://stackoverflow.com/a/14061796. See if I like this.
# If the first argument is "check",
ifeq (check,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "check", and turn into do-nothing targets
  CHECK_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(CHECK_ARGS):;@:)
endif

check: $(TEST_DEPENDENCIES)
	./run_tests $(CHECK_ARGS)


# For the `|` symbol: https://stackoverflow.com/q/12299369
$(CLIENT_OBJ)/%.o: $(CLIENT_SRC)/%.cpp $(CLIENT_SRC)/simple_client.hpp $(CLIENT_SRC)/shardkv_client.hpp | $(CLIENT_OBJ)
	$(CC) $(CPPFLAGS) -c $< -o $@

# TODO: fix dependencies for header only files
$(COMMON_OBJ)/%.o: $(COMMON_SRC)/%.cpp $(COMMON_SRC)/%.hpp | $(COMMON_OBJ)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(KVSTORE_OBJ)/%.o: $(KVSTORE_SRC)/%.cpp $(KVSTORE_SRC)/%.hpp | $(KVSTORE_OBJ)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(NET_OBJ)/%.o: $(NET_SRC)/%.cpp $(NET_SRC)/%.hpp | $(NET_OBJ)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(REPL_OBJ)/%.o: $(REPL_SRC)/%.cpp $(REPL_SRC)/%.hpp | $(REPL_OBJ)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(SERVER_OBJ)/%.o: $(SERVER_SRC)/%.cpp $(SERVER_SRC)/server.hpp | $(SERVER_OBJ)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(SHARDMASTER_OBJ)/%.o: $(SHARDMASTER_SRC)/%.cpp $(SHARDMASTER_SRC)/shardmaster.hpp | $(SHARDMASTER_OBJ)
	$(CC) $(CPPFLAGS) -c $< -o $@



# ===== Testing stuff
$(TEST_UTILS_OBJ)/%.o: $(TEST_UTILS_SRC)/%.cpp $(TEST_UTILS_SRC)/%.hpp | $(TEST_UTILS_OBJ)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(QUEUE_TESTS_OBJ)/%.o: $(QUEUE_TESTS_SRC)/%.cpp | $(QUEUE_TESTS_OBJ)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(KVSTORE_SEQUENTIAL_TESTS_OBJ)/%.o: $(KVSTORE_SEQUENTIAL_TESTS_SRC)/%.cpp | $(KVSTORE_SEQUENTIAL_TESTS_OBJ)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(KVSTORE_PARALLEL_TESTS_OBJ)/%.o: $(KVSTORE_PARALLEL_TESTS_SRC)/%.cpp | $(KVSTORE_PARALLEL_TESTS_OBJ)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(KVSTORE_PERFORMANCE_TESTS_OBJ)/%.o: $(KVSTORE_PERFORMANCE_TESTS_SRC)/%.cpp | $(KVSTORE_PERFORMANCE_TESTS_OBJ)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(KVSTORE_INTEGRATION_TESTS_OBJ)/%.o: $(KVSTORE_INTEGRATION_TESTS_SRC)/%.cpp | $(KVSTORE_INTEGRATION_TESTS_OBJ)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(SHARDMASTER_TESTS_OBJ)/%.o: $(SHARDMASTER_TESTS_SRC)/%.cpp | $(SHARDMASTER_TESTS_OBJ)
	$(CC) $(CPPFLAGS) -c $< -o $@

$(SHARDKV_TESTS_OBJ)/%.o: $(SHARDKV_TESTS_SRC)/%.cpp | $(SHARDKV_TESTS_OBJ)
	$(CC) $(CPPFLAGS) -c $< -o $@

# TODO: eventually, structure these variables in a sane way...
#
# Suffix removal: https://stackoverflow.com/q/16767575
QUEUE_TESTS = $(basename $(QUEUE_TESTS_OBJS))
# Prefix removal: https://stackoverflow.com/q/19571391, automatic target generation: https://stackoverflow.com/a/10172729
$(QUEUE_TESTS:$(QUEUE_TESTS_OBJ)/%=%): %: $(TEST_DEPENDENCIES) $(QUEUE_TESTS_OBJ)/%.o
	$(CC) $(CPPFLAGS) $^ -o $@

KVSTORE_SEQUENTIAL_TESTS = $(basename $(KVSTORE_SEQUENTIAL_TESTS_OBJS))
$(KVSTORE_SEQUENTIAL_TESTS:$(KVSTORE_SEQUENTIAL_TESTS_OBJ)/%=%): %: $(TEST_DEPENDENCIES) $(KVSTORE_SEQUENTIAL_TESTS_OBJ)/%.o
	$(CC) $(CPPFLAGS) $^ -o $@

KVSTORE_PARALLEL_TESTS = $(basename $(KVSTORE_PARALLEL_TESTS_OBJS))
$(KVSTORE_PARALLEL_TESTS:$(KVSTORE_PARALLEL_TESTS_OBJ)/%=%): %: $(TEST_DEPENDENCIES) $(KVSTORE_PARALLEL_TESTS_OBJ)/%.o
	$(CC) $(CPPFLAGS) $^ -o $@

KVSTORE_PERFORMANCE_TESTS = $(basename $(KVSTORE_PERFORMANCE_TESTS_OBJS))
$(KVSTORE_PERFORMANCE_TESTS:$(KVSTORE_PERFORMANCE_TESTS_OBJ)/%=%): %: $(TEST_DEPENDENCIES) $(KVSTORE_PERFORMANCE_TESTS_OBJ)/%.o
	$(CC) $(CPPFLAGS) $^ -o $@

KVSTORE_INTEGRATION_TESTS = $(basename $(KVSTORE_INTEGRATION_TESTS_OBJS))
$(KVSTORE_INTEGRATION_TESTS:$(KVSTORE_INTEGRATION_TESTS_OBJ)/%=%): %: $(TEST_DEPENDENCIES) $(KVSTORE_INTEGRATION_TESTS_OBJ)/%.o
	$(CC) $(CPPFLAGS) $^ -o $@

SHARDMASTER_TESTS = $(basename $(SHARDMASTER_TESTS_OBJS))
$(SHARDMASTER_TESTS:$(SHARDMASTER_TESTS_OBJ)/%=%): %: $(TEST_DEPENDENCIES) $(SHARDMASTER_TESTS_OBJ)/%.o
	$(CC) $(CPPFLAGS) $^ -o $@

SHARDKV_TESTS = $(basename $(SHARDKV_TESTS_OBJS))
$(SHARDKV_TESTS:$(SHARDKV_TESTS_OBJ)/%=%): %: $(TEST_DEPENDENCIES) $(SHARDKV_TESTS_OBJ)/%.o
	$(CC) $(CPPFLAGS) $^ -o $@

.PHONY = all clean check format
