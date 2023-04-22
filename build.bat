@echo off
IF "%GITHUB_ACTIONS%"=="" call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
:: TODO, prob replace with CMake because locating the correct vcvarsall.bat is a pain
mkdir bin

del bin\build.exe

g++ -std=c++11 -Wno-write-strings -Wno-writable-strings -o bin/build ^
	-g util/build.cpp -I src/ -I util/ -I backend/src -I vendor/

"bin\build.exe" %1 %2