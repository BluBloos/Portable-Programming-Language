@echo off

:: TODO: make the above more robust. we should be searching for where vcvarsall.bat is.
:: https://github.com/mmozeiko/wcap/blob/main/build.cmd seem to have a good solution.

mkdir bin
del bin\build.exe

:: NOTE: _CRT_SECURE_NO_WARNINGS disables warnings when using "less secure"
:: functions such as fopen(). I'm fine using these since they may be more
:: performant due to less error checking.
set COMPILER_FLAGS=/Z7 /W3 /D _CRT_SECURE_NO_WARNINGS
set INCLUDE_DIRS=/I ..\src /I ..\src\third_party

pushd bin

cl %INCLUDE_DIRS% %COMPILER_FLAGS% ..\src\ppl.cpp /link /DEBUG:FULL /OUT:ppl.exe

popd