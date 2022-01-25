@echo off

:: Create the bin directory
mkdir bin

:: Compile and run Build
:: NOTE(Noah): $@ is an array of all arguments passed on to build.bat
g++ -std=c++11 -Wno-writable-strings -o bin/build -g util/build.c -I src/ \
	&& bin/build "$@"