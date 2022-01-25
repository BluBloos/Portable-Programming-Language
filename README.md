<div>
<p align="center">
<img src="https://user-images.githubusercontent.com/38915815/147722834-1602909f-fcbc-49bf-8ec3-6a0bd33ef3ec.png" />  
</p>
</div>

The offical repo for the portable programming language. Built with ❤️

## Foreword

This project/language is still heavy in development. 

Currently porting from a Python-based codebase to a C/C++ codebase. Working on codegen to LLVM IR.

## Building

The only dependency as of right now is a C++ compiler.

NOTE: The project has not been tested on any other compilers aside from clang on macOS x86_64. No guarentees for a working build on other systems!

The project uses an interactive build system. Simply run the build.sh script to get started. 

```
./build.sh
```

If you are using Visual Studio Code https://code.visualstudio.com/, just run Shift+Command+B (on macOS) and Shift+Control+B (on Windows).