#! /bin/sh

# Add prefix 'x86_64-linux-gnu-' in ARM; leave prefix blank in x86.

uname -m | grep "arm\|aarch64" >/dev/null && echo "x86_64-linux-gnu-" || echo ""