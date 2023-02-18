# Default optimization level
OPT ?= -O2

# Flags
CXXFLAGS := -std=gnu++1z -W -Wall -Wshadow  -Wno-unused-command-line-argument -g $(OPT) $(DEFS) $(CXXFLAGS)
LDFLAGS := -no-pie

# Make sanitizers available as a compilation option ("make SAN=1")
#
# use SAN=1 to turn on all sanitizers (address, leak, undefined behavior)
#
# to turn on specific sanitizers, use:
# - ASAN=1 for the address sanitizer
# - LSAN=1 for the leak sanitizer
# - UBSAN=1 for the undefined behavior sanitizer
-include sanitizers.mk

# Get all of the test filenames
TESTS = $(patsubst %.cc,%,$(sort $(wildcard test[0-9][0-9][0-9].cc)))

all: $(TESTS)

# Link math library for static functions

LIBS = -lm

# Allow "make V=1" to print verbose compilation output (incl. all commands run)
ifneq ($(V),1)
# Define run function, which echoes its second and third argument and then
# lays its first and third out as a command
# https://www.gnu.org/software/make/manual/html_node/Call-Function.html
run = @$(if $(2),/bin/echo "  $(2) $(3)" &&,) $(1) $(3)
else
run = $(1) $(3)
endif


%.o: %.cc $(BUILDSTAMP)
	$(call run,$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c,COMPILE,$<)

all:
	@echo "*** Run 'make check' to check your work."

test%: dmalloc.o basealloc.o test%.o
	$(call run,$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS),LINK $@)

# Check all tests
check: $(patsubst %,run-%,$(TESTS))
	@echo "*** All tests succeeded!"

# Check a specific test... don't worry about perl
check-%:
	@any=false; good=true; for i in `perl check.pl -e "$*"`; do \
	    any=true; $(MAKE) run-$$i || good=false; done; \
	if $$any; then $$good; else echo "*** No such test" 1>&2; $$any; fi

# Run rule .. different than run function!
run-:
	@echo "*** No such test" 1>&2; exit 1

run-%: %
	@test -d out || mkdir out
	@perl check.pl -x $<

format:
	clang-format -style=Google -i $(FILES)


clean: clean-main
clean-main:
	$(call run,rm -f $(TESTS) *.o core *.core,CLEAN)
	$(call run,rm -rf out *.dSYM $(DEPSDIR))

MALLOC_CHECK_=0
export MALLOC_CHECK_

.PRECIOUS: %.o
.PHONY: all clean clean-main format \
	run run- run% check  check-%
