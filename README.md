# Plasma Compiler ⚡
Friendly C compiler written in Python. It does not comply with any standards for the C language, but man can this thing compile, just check out the examples in the tests folder!

## Details
I lied, it's actually a transpiler, it converts the C source to x86 assembly. Third party tools assemble and link the program. The code is fully custom, everything from the lexer to code generation. 

## Build Steps
- NOTE: The compiler is only supported to run on Windows host machines, and it will only compile win32 binaries

This project uses the MSVC linker, so you will need to install <a href="https://visualstudio.microsoft.com/vs/">Visual Studio</a> if you haven't already. At the time of writing, the latest version is 2019, so I cannot guarentee proper linking for any subsequent versions. Also note that if your visual studio version is different than 2019, you will need to change *shell.bat* accordingly. In addition, this project uses Netwide Assembler (<a href="https://www.nasm.us/">NASM</a>). Install the binaries and make sure the bin directory is set in the system environment variables.    

After installing these, clone this project and run the following commands. This will run the compiler against the premade test suite. You are free to add more tests if you like. 
```
$ shell.bat
$ python test.py
```

## Features
- Single line comments
- Function declaration
- Variables and Scoping
- Variables assignment
- If, else if, and else
- For loops
- While loops
- Break / Continue
- Recursion
- Precedence in expressions
- Ternary operator
- Supported binary operations: +   -    *    /    ==    !=    >    <    >=    <=    ||    &&
- Supported unary operations: - !

## Features on the radar
- Arrays
- Pointers
- Standard library
- etc...
