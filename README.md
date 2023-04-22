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

Built with ❤️, this repository contains the official compiler toolchain for the Portable Programming Language.

PPL is a systems programming language with the goal of allowing the developer to produce software, hassle-free, for any
target platform.

Here is a small sample of PPL,

```c
pc : #import "ppl.console";
#import "ppl.array";

main : () -> int {
    myIntegers : array<ui32> = {};

    i := 0u;
    for i < 10 {
        myIntegers.push(i++);
    }

    pc.print("\nThere are %d many integers\n", myIntegers.size);

    return 0;
}
```

This language takes inspiration from many other programming languages. To list them all would be much too exhaustive.
Notable mentions would be C, C++, and JAI.

# Foreword

This project is in HEAVY DEVELOPMENT.

The language is undergoing design, and the compiler is incomplete. All that is possible at the moment is to run unit and
integration tests for components of the compiler.

See https://github.com/BluBloos/Portable-Programming-Language/wiki for devlogs.

# Steps for Building

The project currently depends on the netwide assembler (NASM), the MSVC linker, and `ld` on macOS.

Support for different setups is still "rough around the edges". No guarantees.

The project uses an interactive build system. Run the following command to launch said system (macOS, Linux).

```bash
./build.sh
```

On Windows, run `build.bat`.

The interactive build system begins by printing a list of all available commands, then waits for subsequent user input.
