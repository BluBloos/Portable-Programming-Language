@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

cl /c /FAs test.cpp

nasm -f win64 ..\pstdlib\Windows\console\exit.s -o ..\..\bin\exit.obj
nasm -f win64 test.s -o ..\..\bin\test.obj
link /subsystem:console /entry:start ..\..\bin\test.obj ..\..\bin\exit.obj /OUT:..\..\bin\test.exe Kernel32.lib

..\..\bin\test.exe

echo test.exe output is %errorLevel%