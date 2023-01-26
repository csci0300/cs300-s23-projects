#!/bin/bash

make snake && gdb  -iex="target remote localhost:12345"

