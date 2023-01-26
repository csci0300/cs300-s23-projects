import os
import subprocess
import re
import json
import sys
import codecs

AUTOGRADER_BINARY = "./autograder"

TRACE_FILE = "test/traces.json"

HEADER = "\033[95m"
OKBLUE = "\033[94m"
OKCYAN = "\033[96m"
OKGREEN = "\033[92m"
WARNING = "\033[93m"
FAIL = "\033[91m"
ENDC = "\033[0m"
BOLD = "\033[1m"
UNDERLINE = "\033[4m"


def ensure_keys_exist(dictionary, keys):
    """Ensure that all keys in `keys` exist in `dictionary`"""
    for key in keys:
        if key not in dictionary:
            return False
    return True


def color(s, color):
    """Color a string via escape codes"""
    return color + s + ENDC


def eprint(s):
    print(color("Error: ", FAIL) + s)


def print_mismatch(name, got, expected):
    """Print a mismatch message for test paramater `name`"""
    print(color(name, BOLD) + " mismatch:")
    print("\t" + color("Got: ", FAIL) + str(got))
    print("\t" + color("Expected: ", OKGREEN) + str(expected))


def print_board_mismatch(got, expected, width, height):
    """Show a diff between two boards, `got` and `expected`, both of width `width` and height `height`"""

    # Replace `X` with unicode block character
    got = got.replace("X", "█")
    expected = expected.replace("X", "█")

    # Record all coordinates where the boards differ
    mismatch_coords = []
    for i in range(height):
        for j in range(width):
            if (i * width + j) >= len(got):
                break
            if expected[i * width + j] == "?":
                # `?` is a wild card that allows any output
                continue
            if got[i * width + j] != expected[i * width + j]:
                mismatch_coords.append((i, j))

    if len(mismatch_coords) == 0:
        return False

    print(color("board", BOLD) + " mismatch:")

    if len(got) != width * height:
        print(
            color("Warning:", WARNING),
            "length of `cells` field of board struct is not equal to `width` * `height`. Below printout may be incoherent.",
        )

    # Print the boards, highlighting the mismatched cells
    print("\tGot: ")
    for i in range(height):
        print("\t", end="")
        for j in range(width):
            if (i * width + j) >= len(got):
                break
            if (i, j) in mismatch_coords:
                print(color(got[i * width + j], FAIL), end="")
            else:
                print(got[i * width + j], end="")
        print()

    print("\tExpected: ")
    for i in range(height):
        print("\t", end="")
        for j in range(width):
            if (i, j) in mismatch_coords:
                print(color(expected[i * width + j], OKGREEN), end="")
            else:
                print(expected[i * width + j], end="")
        print()

    return True


def run_test(test_name, test_parameters):
    """Run the test indicated by `test_parameters`"""

    # ensure all input test parameters are present
    if not ensure_keys_exist(
        test_parameters, ["seed", "key_input", "snake_grows", "output"]
    ):
        eprint("missing test input parameters")
        sys.exit(1)

    # ensure all output test parameters are present
    if not ensure_keys_exist(
        test_parameters["output"], ["game_over", "score", "width", "height", "cells"]
    ) and not ensure_keys_exist(test_parameters["output"], ["board_error"]):
        eprint("missing test output parameters")
        sys.exit(1)

    # only do the following setup if this test has an expected board output
    if test_parameters["output"].get("cells") is not None:
        # Collapse output cell 2d array to 1d string
        test_parameters["output"]["cells"] = "".join(test_parameters["output"]["cells"])
        # Ensure that output board cells are well-formed
        if (
            len(test_parameters["output"]["cells"])
            != test_parameters["output"]["width"] * test_parameters["output"]["height"]
        ):
            eprint(
                "invalid test case. Length of `cells` is not equal to `width` * `height`"
            )
            sys.exit(1)

    # if test specifies name, output should also specify name and length
    if "name" in test_parameters:
        if not ensure_keys_exist(test_parameters["output"], ["name", "name_len"]):
            eprint("missing test output parameters (name and/or name_len)")
            sys.exit(1)


    # Set up pipes so the autograder can send results back to us
    (r, w) = os.pipe()
    os.set_inheritable(r, True)
    os.set_inheritable(w, True)

    # Run the autograder via a subprocess
    print("Running test", test_name),
    results = subprocess.run(
        [
            AUTOGRADER_BINARY,
            "0"
            if test_parameters.get("board") is None
            else test_parameters.get("board"),
            test_parameters.get("seed"),
            test_parameters.get("snake_grows"),
            test_parameters.get("key_input"),
            "1" if "name" in test_parameters else "0",
            str(w),
        ],
        close_fds=False,
        input=(
            test_parameters.get("name", "") + "\n"
        ).encode(),
    )
    if results.returncode != 0:
        results.returncode
        eprint("Failure running student code. Marking as failure.")
        return False

    # Get the expected output from the test file
    expected_output = test_parameters.get("output")

    # Get the actual output by reading the results from the pipe that were sent by the autograder
    os.close(w)
    r = os.fdopen(r)
    actual_output = json.loads(r.read())

    # Loop over keys in the expected output and actual output and print any discrepencies
    failure = False


    expected_error = expected_output.get("board_error")
    actual_error = actual_output.get("board_error")
    if expected_error is not None and actual_error is not None:
        if expected_error != actual_error:
            failure = True
            print_mismatch("board error", actual_error, expected_error)
    elif expected_error is not None and actual_error is None:
        failure = True
        print_mismatch("board error", "success", expected_error)
    elif expected_error is None and actual_error is not None:
        failure = True
        print_mismatch("board error", actual_error, "success")
    else:
        keys_to_compare = ["game_over", "score", "width", "height"]
        for key in keys_to_compare:
            expected = expected_output[key]
            actual = actual_output[key]
            if expected != actual:
                print_mismatch(key, actual, expected)
                failure = True

        # We special case checking for mismatches in the boards so that we can pretty print
        failure = (
            print_board_mismatch(
                actual_output["cells"],
                expected_output["cells"],
                expected_output["width"],
                expected_output["height"],
            )
            or failure
        )

        # If name was specified, test name
        if "name" in test_parameters:
            expected_name = expected_output["name"]
            name_bytes = actual_output["name"]

            decode_hex = codecs.getdecoder("hex_codec")
            try:
                output_name = (decode_hex(name_bytes)[0]).decode("utf-8")
                if output_name != expected_name:
                    print_mismatch("name", output_name, expected_name)
            except Exception as e:
                print("Got " + str(type(e)) + " exception while decoding output name data!")
                print(e)
                print_mismatch("name", "bytes " + name_bytes, expected_name)
                failure = True

            keys_to_compare = ["name_len"]
            for key in keys_to_compare:
                expected = expected_output[key]
                actual = actual_output[key]
                if expected != actual:
                    print_mismatch(key, actual, expected)
                    failure = True

    if failure:
        print(color("Test failed. See above for details", FAIL))
        print("\tTest purpose: ", test_parameters["description"])
        return False
    else:
        print(color("Test passed successfully", OKGREEN))
        return True

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 autograder.py <test_number> ...")
        sys.exit(1)

    # open the trace file
    try:
        with open(TRACE_FILE, "r") as trace_file:
            traces = json.loads(trace_file.read())
    except OSError:
        eprint("could not open trace file")
        sys.exit(1)

    test_numbers = sys.argv[1:]

    test_names = [f"test%03d" % int(n) for n in test_numbers]

    passed = []
    failed = []
    for test_name in test_names:
        trace = traces.get(test_name)
        if trace is None:
            eprint(f"could not find test {test_name}")
            sys.exit(1)
        succeeded = run_test(test_name, trace)
        if succeeded:
            passed.append(test_name)
        else:
            failed.append(test_name)

        print()
    total = len(passed) + len(failed)
    passed_percent = (len(passed) / total) * 100
    failed_percent = (len(failed) / total) * 100
    print("=" * 30, "SUMMARY", "=" * 30)
    print("PASSED:", ", ".join(passed), "|", passed_percent, "%")
    print("FAILED:", ", ".join(failed), "|", failed_percent, "%")

main()
