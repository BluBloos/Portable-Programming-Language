![](https://github.com/BluBloos/Portable-Programming-Language/workflows/macOS%20build/badge.svg)
![](https://github.com/BluBloos/Portable-Programming-Language/workflows/Ubuntu%20build/badge.svg)
![](https://github.com/BluBloos/Portable-Programming-Language/workflows/Windows%20build/badge.svg)

<div>
  <p align="center">
  <img src="https://user-images.githubusercontent.com/38915815/147722834-1602909f-fcbc-49bf-8ec3-6a0bd33ef3ec.png" />  
  </p>
</div>

The offical repo for the portable programming language. Built with ❤️

## Foreword

This project/language is still heavy in development.

Currently porting from a Python-based codebase to a C/C++ codebase. Working on codegen to LLVM IR. See <a href="/TODO.md">TODO.md</a> for more details.

## Building

The only dependency as of right now is a C++ compiler. 

The project has been tested with the following setups
- Clang on macOS x86_64
- MinGW-W64 and Git bash on Windows
- g++ on Ubuntu 20.04.3

Support for different setups is still "rough around the edges". No guarentees.

The project uses an interactive build system. Simply run build.sh to get started.
