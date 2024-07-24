#!/bin/sh

# Create the bin directory
mkdir bin

# Compile and run Build
# NOTE(Noah): $@ is an array of all arguments passed on to build.sh
g++ -std=c++11 -Wall -Wno-unused-value -Wno-write-strings -Wno-writable-strings -o bin/ppl \
	-g src/ppl.cpp -I src/ -I src/third_party