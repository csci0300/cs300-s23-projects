# are we using clang?
ISCLANG := $(shell if $(CC) --version | grep -e 'LLVM\|clang' >/dev/null; then echo 1; fi)
ISLINUX := $(if $(wildcard /usr/include/linux/*.h),1,)

# sanitizer arguments
#
# to turn on all sanitizers (address, leak, undefined behavior), use SAN=1
#
# to turn on specific sanitizers, use:
# - ASAN=1 for the address sanitizer
# - LSAN=1 for the leak sanitizer
# - UBSAN=1 for the undefined behavior sanitizer
ifndef SAN
SAN := $(SANITIZE)
endif

check_for_sanitizer = $(if $(strip $(shell $(CC) -fsanitize=$(1) -x c -E /dev/null 2>&1 | grep sanitize=)),$(info ** WARNING: The `$(CC)` compiler does not support `-fsanitize=$(1)`.),1)
ifeq ($(or $(ASAN),$(SAN)),1)
 ifeq ($(call check_for_sanitizer,address),1)
CFLAGS += -fsanitize=address
CXXFLAGS += -fsanitize=address
 endif
endif
ifeq ($(or $(LSAN),$(LEAKSAN)),1)
  ifeq ($(call check_for_sanitizer,leak),1)
CFLAGS += -fsanitize=leak
CXXFLAGS += -fsanitize=leak
 endif
endif
ifeq ($(or $(UBSAN),$(SAN)),1)
 ifeq ($(call check_for_sanitizer,undefined),1)
CFLAGS += -fsanitize=undefined
CXXFLAGS += -fsanitize=undefined
 endif
endif
