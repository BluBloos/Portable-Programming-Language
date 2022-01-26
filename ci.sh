#!/bin/sh

# Compile and run Build
# NOTE(Noah): $@ is an array of all arguments passed on to build.sh
g++ -std=c++11 -Wno-writable-strings -o bin/ci_linter -g util/ci_linter.c -I src/ -I util/ \
	&& bin/ci_linter "$@"