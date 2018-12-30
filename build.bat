@echo off
python compiler.py test.c
nasm -f elf test.c.asm -o test.o
gcc test.o -o test.exe
python run.py