@echo off
IF "%GITHUB_ACTIONS%"=="" call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

mkdir bin

g++ -std=c++11 -Wno-write-strings -Wno-writable-strings -o bin/build ^
	-g util/build.c -I src/ -I util/ -I backend/src -I vendor/

"bin\build.exe" %1