#!/bin/sh

# Create the bin directory
mkdir bin

# Compile and run Build
# NOTE(Noah): $@ is an array of all arguments passed on to build.sh
gcc -o bin/build -g util/build.c \
	&& bin/build "$@"