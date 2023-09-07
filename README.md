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

Built with <3, this package contains the official compiler toolchain for the
Portable Programming Language.

PPL is a systems programming language with the goal of allowing the developer
to produce software, hassle-free, for any target platform.

Here is a small sample of PPL,

```c
pal  :: #import "ppl.pal";
plib :: #import "ppl.types";

main :: () -> int {
    myIntegers := plib.Array<u32> {};

    for i in 0 ..< 10 {
        myIntegers.push(i);
    }

    pal.print("\nThere are % many integers\n", myIntegers.size);

    return 0;
}
```

This language takes inspiration from many other programming languages. To list
them all would be much too exhaustive. Notable mentions would be C/C++, Jai,
and Odin.

# Foreword

This project is in HEAVY DEVELOPMENT.

The language is undergoing active design. The design currently leads while the
compiler support lags behind.

# Steps for Building and Using

The project uses an interactive build system. The interactive build system
begins by printing a list of all available commands, then waits for
subsequent user input.

## macOS

Dependency: `ld` must be available from the terminal. 

Run the following command to launch the interactive build system.

```bash
./build.sh
```

## Windows

Dependency: MSVC linker.

Run the following command to launch the interactive build system.

```batch
build.bat
```
