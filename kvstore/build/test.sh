#!/bin/bash

# ANSI color codes
RED='\033[0;31m'
YELLOW='\033[1;33m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
GRAY='\033[0;37m'
NC='\033[0m' # No color

VERBOSE=1
SHORT_CIRCUIT=0
TIMEOUT=30

# SCRIPT CONSTANTS

TESTS_DIR="../tests"

TEST_SECTIONS=(
    "A1"
    "A2"
    "A3"
    "A4"
    "A5"
)

TEST_DIRS=("queue_tests" "kvstore_sequential_tests" "kvstore_parallel_tests" "kvstore_performance_tests" "kvstore_integration_tests")

declare -A SECTION_DIRS
SECTION_DIRS["A1"]="kvstore_sequential_tests"
SECTION_DIRS["A2"]="kvstore_parallel_tests"
SECTION_DIRS["A3"]="queue_tests"
SECTION_DIRS["A4"]="kvstore_sequential_tests"
SECTION_DIRS["A5"]="kvstore_parallel_tests"
SECTION_DIRS["B1"]=""
SECTION_DIRS["B2"]="shardmaster_tests"
SECTION_DIRS["B3"]="shardkv_tests"

declare -A SECTION_ARGS
SECTION_ARGS["A1"]="simple"
SECTION_ARGS["A2"]="simple"
SECTION_ARGS["A3"]="simple"
SECTION_ARGS["A4"]="concurrent"
SECTION_ARGS["A5"]="concurrent"
SECTION_ARGS["B1"]=""
SECTION_ARGS["B2"]=""
SECTION_ARGS["B3"]=""

EXTENSION="cpp"
TSAN=""
ASAN=""

# tempfiles and cleanup
TMP_STDOUT="$(pwd)/$(mktemp ./tmp_out.XXX)"
TMP_STDERR="$(pwd)/$(mktemp ./tmp_err.XXX)"
TMP_RETURN="$(pwd)/$(mktemp ./tmp_ret.XXX)"
cleanup(){ rm -f "$TMP_STDOUT" "$TMP_STDERR" "$TMP_RETURN"; return; }

# Cleanup and kill on Ctrl-C
trap '
  trap - INT # restore default INT handler
  cleanup
  kill -s INT "$$"
' INT



usage(){
	echo "Usage: $0 [-hv]"
	echo "       run tests"
}

display(){
	if [ $# -eq 1 ]; then
		LEVEL=0
		STRING="$1"
	elif [ $# -eq 2 ]; then
		LEVEL="$1"
		STRING="$2"
	fi

	if [ $VERBOSE -gt "$LEVEL" ]; then
		echo -e "$STRING"
	fi
}

display_sameline(){
	if [ $# -eq 1 ]; then
		LEVEL=0
		STRING="$1"
	elif [ $# -eq 2 ]; then
		LEVEL="$1"
		STRING="$2"
	fi

	if [ $VERBOSE -gt "$LEVEL" ]; then
		echo -ne "$STRING"\\r
	fi
}

source_if_exists(){
	if [ -f "$1" ]; then
		source "$1" > /dev/null
	fi
}


run_test(){
    TEST_SECTION="$1"
	TEST_NAME="$2"
	TEST_NUMBER="$3"
	EXEC="./$TEST_NAME"
    ARGS="$4"

	# if no command file, don't do test
	if [ ! -f "$EXEC" ]; then
		echo "ERROR: $EXEC not found" >&2
		return 2
	fi

	# get expected return code, or 0 as default
	EXPECTED_RET=0

	display_sameline "$TEST_NUMBER. [  ......  ] ${YELLOW}$TEST_NAME${NC}"

	# run command
	(
	  echo "" > "$TMP_STDOUT"
	  echo "test timed out" > "$TMP_STDERR"
		# run command, capture stdout, stderr
		timeout "$TIMEOUT"s "$EXEC" "${ARGS[*]}" > "$TMP_STDOUT" 2> "$TMP_STDERR"
		# capture return code
		echo $? > "$TMP_RETURN"
		exit 0
        # run command, capture stdout, stderr
        # "$EXEC" > "$TMP_STDOUT" 2> "$TMP_STDERR"
        # capture return code
        # echo $? > "$TMP_RETURN"
	)

	OUTPUT=$(cat "$TMP_STDOUT")
	ERROR_OUTPUT=$(cat "$TMP_STDERR")
	RET=$(cat "$TMP_RETURN")

	# compare return code
	if [[ "$RET" != "$EXPECTED_RET" ]]; then
        REASON="FAILED"
        if [[ "$RET" == 124 ]]; then
          REASON="-TIMED"
        fi
		display "$TEST_NUMBER. [  ${RED}$REASON${NC}  ] ${YELLOW}$TEST_NAME${NC}"
		if [ -n "$ERROR_OUTPUT" ]; then display "${RED}STDERR:${NC} $ERROR_OUTPUT"; display ""; fi
		return 1
	fi

	# if we reached here, test passed
	display "$TEST_NUMBER. [  ${GREEN}PASSED${NC}  ] ${YELLOW}$TEST_NAME${NC}"
}

test_section() {
	SECTION=$1

    # For each test directory for that section:
    for DIR in ${SECTION_DIRS[$1]}; do
        DIR_SPLITTER=" ${GRAY}==${NC} ${GRAY}$DIR${NC} ${GRAY}==${NC}"
        display "$DIR_SPLITTER"

        # Parse test names
        TESTS_IN_SECTION=$(ls "$TESTS_DIR/$DIR" | grep ".$EXTENSION")
        TESTS_IN_SECTION=$(for t in ${TESTS_IN_SECTION}; do echo "${t%."$EXTENSION"}"; done)

        # Run each test
        for t in ${TESTS_IN_SECTION}; do
            # Build test
            display_sameline "$TEST_NUMBER. [ BUILDING ] ${YELLOW}$t${NC}"
            make $TSAN $ASAN "$t" > "/dev/null" 2> "$TMP_STDERR"
            RET=$?

            # Check if build failed
            if [ ! "$RET" -eq  0 ]; then
                # BUILD_FAILED=1
                display "[${RED}BUILD FAILURE${NC}] ${YELLOW}$t${NC}"
                cat "$TMP_STDERR"
                if [ "$SHORT_CIRCUIT" -eq 1 ]; then
                  return 1
              fi
            fi

            run_test "$SECTION" "$t" "$TEST_NUMBER" "${SECTION_ARGS[$1]}"
            RET=$?

            # udpate passed and failed
            if [ "$RET" -eq  0 ]; then
                NUM_PASSED=$((NUM_PASSED+1))
            elif [ "$RET" -eq  1 ]; then
                NUM_FAILED=$((NUM_FAILED+1))
                # Short curcuit if enabled
                if [ "$SHORT_CIRCUIT" -eq 1 ]; then
                  return 1
                fi
            fi

            TEST_NUMBER=$((TEST_NUMBER+1))
        done
	done
}

clean_all_tests() {
	echo "Removing all test executables..."
    for DIR in "${TEST_DIRS[@]}"; do
		TESTS_IN_SECTION=$(ls "$TESTS_DIR/$DIR" | grep ".$EXTENSION")
		TESTS_IN_SECTION=$(for t in ${TESTS_IN_SECTION}; do echo "${t%."$EXTENSION"}"; done)
		for t in ${TESTS_IN_SECTION}; do
			TEST_FILE="$TESTS_DIR/$DIR/$t"
			if [ -f "$TEST_FILE" ]; then
				echo "rm -f $TEST_FILE"
				rm "$TEST_FILE"
			fi
		done
	done
}

count_tests() {
    COUNT=0
    for SECTION in "${TEST_SECTIONS[@]}"; do
        for DIR in ${SECTION_DIRS[$SECTION]}; do
            TESTS_IN_SECTION=$(ls "$TESTS_DIR/$DIR" | grep ".$EXTENSION")
            TESTS_IN_SECTION=$(for t in ${TESTS_IN_SECTION}; do echo "${t%."$EXTENSION"}"; done)
            for t in ${TESTS_IN_SECTION}; do
                COUNT=$((COUNT+1))
            done
        done
    done
	echo $COUNT
}

# I think this can be unused, if we allow for multiple sections in the command line.
update_sections(){
    SECTION=$1
    for s in "${TEST_SECTIONS[@]}"; do
        if [ "$s" == "$SECTION" ]; then
            TEST_SECTIONS+=("$SECTION")
            return
        fi
    done
    echo -e "${RED}ERROR${NC}: Did not find section ${RED}$SECTION${NC}"
    echo
    echo -e "${BLUE}Sections:${BLUE}"
    for s in "${TEST_SECTIONS[@]}"; do
        echo -e "${YELLOW}$s${NC}"
    done
    exit 1
}

POSITIONAL=()
while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        -q|--quiet)
        VERBOSE=0
        shift # Remove --initialize from processing
        shift # TODO: looks like the previous version used 2 shifts here?
        ;;
        -t|--timeout)
        TIMEOUT="$2"
        shift # past argument
        shift # past value
        ;;
        -s|--short)
        SHORT_CIRCUIT=1
        shift # past argument
        ;;
        -c|--clean)
        clean_all_tests && exit 0
        shift # Remove --initialize from processing
        ;;
        -h|--help)
        usage && exit 0
        shift # Remove argument name from processing
        ;;
        -z|--tsan)
        TSAN="TSAN=1"
        echo "Thread sanitizer enabled."
        shift # Remove argument name from processing
        ;;
        -a|--asan)
        ASAN="ASAN=1"
        echo "Address sanitizer enabled."
        shift # Remove argument name from processing
        ;;
        *)
        POSITIONAL+=("$1")
        shift # Remove generic argument from processing
        ;;
    esac
done

set -- "${POSITIONAL[@]}" # restore positional parameters

# If number of arguments > 0, then section(s) have been specified
if [ $# -ge 1 ]; then
    # Iterate over each argument, and check that each section actually exists.
    for SECTION in "$@"; do
        if [[ "${TEST_SECTIONS[*]}" != *"$SECTION"* ]]; then
            echo -e "${RED}ERROR${NC}: Did not find section ${RED}$SECTION${NC}"
            echo
            echo -e "${BLUE}Sections:${BLUE}"
            for s in "${TEST_SECTIONS[@]}"; do
                echo -e "${YELLOW}$s${NC}"
            done
            exit 1
        fi
    done

    # If successful, clear test sections, then add each section to the list
    TEST_SECTIONS=()
    for SECTION in "$@"; do
        TEST_SECTIONS+=("$SECTION")
    done
fi

NUM_TESTS=$(count_tests)
NUM_PASSED=0
NUM_FAILED=0
TEST_NUMBER=1

for section in "${TEST_SECTIONS[@]}"; do
	TEST_SPLITTER=" ${BLUE}===${NC} ${YELLOW}$section${NC} ${BLUE}===${NC}"
	display "$TEST_SPLITTER"
	test_section "$section"
	RET=$?
	display ""
	if [ "$RET" -eq 1 ] && [ "$SHORT_CIRCUIT" -eq 1 ]; then
	  break
	fi
done

SPLITTER="${BLUE}=======================${NC}"

echo -e "$SPLITTER"
echo -e "${NC}Tests Passed:${NC} ${YELLOW}$NUM_PASSED${NC} / ${YELLOW}$NUM_TESTS${NC}"
echo -e "$SPLITTER"

cleanup
