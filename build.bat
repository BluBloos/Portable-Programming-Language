:: Create the bin directory
mkdir bin

:: Compile and run Build
:: NOTE(Noah): $@ is an array of all arguments passed on to build.sh
g++ -std=c++11 -Wno-writable-strings -Wno-write-strings -o bin/build -g util/build.c -I src/ -I util/ && bin\build.exe "$@"