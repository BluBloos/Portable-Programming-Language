@echo off

:: TODO: make the above more robust. we should be searching for where vcvarsall.bat is.
:: https://github.com/mmozeiko/wcap/blob/main/build.cmd seem to have a good solution.

mkdir bin

del bin\build.exe

:: GNU compiler seems to not understand that if I do e.g. a `++` postfix that this has side effects,
:: yet it screams at me of an "unused value". Hence, we use -Wno-unused-value.

:: TODO: maybe we care about being "correct". so converting a compile-time string to a `char *` is
:: deprecated behavior in C++. Yes we enable it with -Wno-write-strings. -Wno-writable-strings is for
:: Clang.

pushd bin

cl /Z7 /W4 /I ..\src /I ..\src\third_party ..\src\ppl.cpp /link /DEBUG:FULL  /OUT:ppl.exe

popd