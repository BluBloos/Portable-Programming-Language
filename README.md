

<div>
  <p align="center">
    <img src="https://user-images.githubusercontent.com/38915815/155840268-844e6655-2554-4ac0-bacb-4d789f980b58.png" />
   

  </p>
  <p align="center">
    <img src="https://github.com/BluBloos/Portable-Programming-Language/workflows/macOS%20build/badge.svg"></img>
    <img src="https://github.com/BluBloos/Portable-Programming-Language/workflows/Ubuntu%20build/badge.svg"></img>
    <img src="https://github.com/BluBloos/Portable-Programming-Language/workflows/Windows%20build/badge.svg"></img>
  </p>
</div>


# Portable Programming Language

Built with ❤️, this repository contains the offical compiler toolchain for the Portable Programming Language. 

The language aims to be general purpose, allowing the user to write software projects as diverse as operating systems, video games, websites, apps, etc. 

The primary aim of the language is to allow the developer to produce software for any target platform, hassle-free. Just write a text file (or many), run the compiler on the command line, and boom - you've got an executable (or bundle, in the case of mobile apps).

Here is a small sample of the language

```c
#import <ppl.console> as pc
int main() {
    [dynamic]int myIntegers;
    for (int i = 0; i < 10; i++) {
        myIntegers.push(i);
    }
    pc::print("There are %d many integers", len(myIntegers));
}
```

As is clear, the language is very C-like. For C and Java programmers (and potentially others), there will be little to no learning curve.

# Foreword

This project is still in heavy development...the language is still being designed, as is the compiler to support the language. 

At the time of writing, the compiler is not "complete". It is not possible to take a PPL source file, run the compiler on it, and produce a working executable for the desired target machine. All that is possible at the moment is to run unit and integration tests for components of the compiler. 

See https://github.com/BluBloos/Portable-Programming-Language/wiki for devlogs.

# Steps for Building

The only dependency as of right now is a C++ compiler. 

The project has been tested with the following setups
- Clang on macOS x86_64
- MinGW-W64 and Git bash on Windows
- g++ on Ubuntu 20.04.3

Support for different setups is still "rough around the edges". No guarentees.

The project uses an interactive build system. Run the following command to launch said system (macOS, Linux).

```bash
./build.sh
```

On Windows, run ```build.bat```


The interactive build system begins by printing a list of all available commands, then waits for susequent user input.
