#!/bin/sh

# Create the bin directory
mkdir bin

# Compile and run Build
# NOTE(Noah): $@ is an array of all arguments passed on to build.sh
g++ -std=c++11 -Wall -Wno-unused-value -Wno-write-strings -Wno-writable-strings -o bin/build \
	-g util/build.cpp -I src/ -I util/ -I backend/src -I vendor/ \
	&& bin/build "$@"