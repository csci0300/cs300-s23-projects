#!/bin/bash
make snake && gdbserver :12345 ./snake ${@: 1}

