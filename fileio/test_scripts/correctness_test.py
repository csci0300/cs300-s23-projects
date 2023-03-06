#!/usr/bin/env python3

import subprocess
import sys
import json

def log(msg):
    if '--grader' not in sys.argv:
        print(msg)


def shell_return(shell_cmd, suppress=False):
    if not suppress:
        log('-> ' + shell_cmd)
    return subprocess.run(
        shell_cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        shell=True
    ).returncode

def files_same(file1, file2):
    return shell_return(f'diff {file1} {file2}', suppress=True) == 0


def byte_cat(infile, outfile, outfile2):
    return shell_return(f'./byte_cat {infile} {outfile}') == 0 \
        and files_same(infile, outfile)

def reverse_byte_cat(infile, outfile, outfile2):
    return shell_return(f'./reverse_byte_cat {infile} {outfile}') == 0 \
        and shell_return(f'./reverse_byte_cat {outfile} {outfile2}') == 0 \
        and files_same(infile, outfile2)

def create_block_cat(block_size):
    def block_cat(infile, outfile, outfile2):
        return shell_return(f'./block_cat {block_size} {infile} {outfile}') == 0 \
            and files_same(infile, outfile)
    return block_cat

def create_reverse_block_cat(block_size):
    def reverse_block_cat(infile, outfile, outfile2):
        return shell_return(f'./reverse_block_cat {block_size} {infile} {outfile}') == 0 \
            and not files_same(infile, outfile) \
            and shell_return(f'./reverse_block_cat {block_size} {outfile} {outfile2}') == 0 \
            and files_same(infile, outfile2)
    return reverse_block_cat

def random_block_cat(infile, outfile, outfile2):
    return shell_return(f'./random_block_cat {infile} {outfile}') == 0 \
        and files_same(infile, outfile)

def rot13(infile, outfile, outfile2):
    assert shell_return(f'cp {infile} {outfile}') == 0
    return shell_return(f'./rot13 {outfile}') == 0 \
        and shell_return(f'./rot13 {outfile}') == 0 \
        and files_same(infile, outfile)



def runtests(tests):
    infile = '/tmp/infile'
    outfile = '/tmp/outfile'
    outfile2 = '/tmp/outfile2'
    integrity = '/tmp/integrity'
    assert shell_return(f'touch {infile}', suppress=True) == 0
    assert shell_return(f'dd if=/dev/urandom of={infile} bs=4096 count=20', suppress=True) == 0
    assert shell_return(f'touch {integrity}', suppress=True) == 0
    assert shell_return(f'cp {infile} {integrity}', suppress=True) == 0

    for test in tests:
        # Truncate output files
        assert shell_return(f'> {outfile}', suppress=True) == 0
        assert shell_return(f'> {outfile2}', suppress=True) == 0

        log(f'\033[32mrunning test: {test}\033[0m')

        testfun = tests[test]
        if test == "ascii_independence":
            assert shell_return(f'touch man_nonascii.txt', suppress=True) == 0
            f = open("man_nonascii.txt", 'w')
            f.write("Make\x00sure\x00your\x00cache\x00can\x00handle\x00null\x00bytes! 가정하는 것은 안전하지 않습니다 प्रत्येकं पात्रं इति 'n ASCII-karakter.")
            f.close()
            assert shell_return(f'touch man_integrity', suppress=True) == 0
            assert shell_return(f'cp man_nonascii.txt man_integrity', suppress=True) == 0

            passed = testfun("man_nonascii.txt", outfile, outfile2)
            if not files_same("man_nonascii.txt", "man_integrity"):
                print('oops, your program modified the input file')
                tests[test] = False
            else:
                tests[test] = True

            assert shell_return(f'rm -- man_nonascii.txt man_integrity', suppress=True) == 0
        else:
            passed = testfun(infile, outfile, outfile2)

            if not files_same(infile, integrity):
                print('oops, your program modified the input file')
                tests[test] = False
            else:
                tests[test] = passed

    log('Your results follow, indicating if each test passed or failed.')
    log('If a test failed, you should debug with a small sample file to see how your output is different!')
    log('Also, if there is sanitizer output, that\'s a good place to start.')
    log('======= CORRECTNESS RESULTS =======')

    if "--grader" not in sys.argv:
        for (test, passed) in tests.items():
            if passed:
              print("{}: \033[32;1mPASSED\033[0m".format(test))
            else:
              print("{}: \033[31;1mFAILED\033[0m".format(test))
    else:
        print(json.dumps(tests, indent=4))

    assert shell_return(f'rm -- {infile} {outfile} {outfile2} {integrity}', suppress=True) == 0


runtests({
    'byte_cat': byte_cat,
    'reverse_byte_cat': reverse_byte_cat,
    'block_cat_1': create_block_cat(1),
    'ascii_independence' : create_block_cat(17),
    'block_cat_17': create_block_cat(17),
    'block_cat_334': create_block_cat(334),
    'block_cat_huge': create_block_cat(8192),
    'block_cat_gargantuan': create_block_cat(32768),
    'reverse_block_cat_1': create_reverse_block_cat(1),
    'reverse_block_cat_13': create_reverse_block_cat(13),
    'reverse_block_cat_987': create_reverse_block_cat(987),
    'reverse_block_cat_huge': create_reverse_block_cat(8192),
    'reverse_block_cat_gargantuan': create_reverse_block_cat(32768),
    'random_block_cat': random_block_cat,
    'rot13': rot13
})
