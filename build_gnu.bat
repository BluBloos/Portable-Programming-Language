@echo off

mkdir bin

del bin\build.exe

:: GNU compiler seems to not understand that if I do e.g. a `++` postfix that this has side effects,
:: yet it screams at me of an "unused value". Hence, we use -Wno-unused-value.

:: TODO: maybe we care about being "correct". so converting a compile-time string to a `char *` is
:: deprecated behavior in C++. Yes we enable it with -Wno-write-strings. -Wno-writable-strings is for
:: Clang.

g++ -std=c++11 -Wall -Wno-unused-value -Wno-write-strings -Wno-writable-strings  -o bin/build ^
	-g util/build.cpp -I src/ -I util/ -I backend/src -I vendor/

"bin\build.exe" %1 %2