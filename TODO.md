# MVP

## Functional Requirements
- Deviates from a toy language
  - Develop real things
    - Things that work on the console
    - Things that have a GUI
    - Things that are hardware accelerated for Graphics
    - Things that are optimized
      - Threading
      - SIMD (Single instruction multiple data)
- Language Design
  - "Joy of programming" metric
- Hit the right developer workflow
  - Syntax highlighting in Vscode
  - Debugging needs to work
  - cli for compiling
- Target Platforms
  - Target the Web
  - Target Mobile 
    - iOS
    - Android
  - Target Desktop operating systems
    - Windows
    - macOS
    - Linux

## The Sensible Road There

I'm gonna start by looking at the things that are a little unclear as to how I might do them. Like, **how do I go about targeting multiple platforms? How do I get debugging to work? How do I reliably support all the needed "real things" across different platforms?**

Let's start the disucssion by thinking about how debugging might work across multiple desktop targets.

Let's break down the components here:
- Source code
- Visual Studio
- The debugger:
  - gdb
  - lldb

The workflow is to compile your code with **debugging information, and this puts this stuff in the symbol table.

But I think this looks different for each platform:**
- Webassembly bytecode
  - Stored in a .wasm file for ingestion by the browser
- DEX file format
  - https://source.android.com/devices/tech/dalvik/dalvik-bytecode
  - Contains a Dalvik bytecode
  - Suitable for being run by the Android Runtime (ART)
- Portable Executable file format
  - Windows
- Mach-O
  - macOS 
  - iOS
- Executable and Linkking format (ELF)
  - Linux

And then for each architecture, do we even know what the instruction set is? Arm, x86, etc? 

For Windows, they run on Intel machines. For macOS, its fuzzy. The older macs use Intel, newer M1 are ARM. iOS is actually ARM. Linux is x86 but I believe can also be compiled to run (and has) on ARM.

For Webassembly, there is a bytecode but also a human-readable form. This is WASM-text and has file ending of .wat

The same goes for the Dalvik bytecode. It also has a human-readable form (and generally x86 and ARM as well).

We will do the following (for the MVP). **x86 for Windows, macOS, and Linux, ARM for iOS, Dalvik for Android, and Webassembly for Web**

Finally, there exists something called the DWARF standard. We might see this in some of the target executable file types.


# ROADMAP

## Port Compiler codebase to C/C++
- Read through the LLVM IR language. Design a slim version and stick to that. 
- Getting to a "Hello, World!"
  - Write a hello world program manually and by hand. 
  - Build codegen to go from this IR to the underlying bytecode for the Intel assembly arch
  - Chuck this in the Mach-O format.
  - Write standard library code for a print routine for macOS.
- In small steps, building up from fundamental grammer components (bottom-up), add codegen to our IR.
- While writing codegen routines, optimize the AST for conciseness and ease of traversal.
- As needed, introduce and write components of the semantic parser.
  - Verify that function names are legal (cannot begin with $, for example).

## Get Language to a "usable" state

In helping to answer this question, we can review the C89 spec http://port70.net/~nsz/c/c89/c89-draft.html#2, as well as existing and randomly chosen projects on Github.

Feature list:
- Can define structs/functions in any scope.
- 0b01010101 (binary literals)
- 0xFF (Hexadecimal literals)
- 1.0e9 (scientific notation)
- default values in structs.
- default parameters in functions OVER function overloading (that's a design choice)
  - Reason being is explicitness.
- struct literals. Ex) v = Vector3{1, 4, 9}
- **Add all desired variable types**
  - dynamic arrays
    - negative indices like Python
  - maps (hash table)
  - string (a utf8 string)
    - you can add strings together.
    - you can concat characters to strings.
    - you can check if two strings are equal.
    - you can index into strings.
    - there exists string literals.
  - char (a unicode codepoint)
  - uin64 (unsigned 64 bit integer), uint32, int32, etc...
  - f32 (32 bit floating point)
  - f64 (64 bit floating point)
  - f128 (128 bit floating point)
  - vector
    - these are math vectors. Matrices are just column vectors with more row sizes > 1.
    - math operations "just work"
- **Add all desired operators**
  - C operator precedence https://en.cppreference.com/w/c/language/operator_precedence, for reference
  - add the len() operator for getting the length of
    - strings
    - dynamic arrays
    - static array
  - Add bitshift operators. Ex) >> and <<.
  - Ensure all postfix and prefix are there Ex) ++X or X++
  - Add -> for dereffing pointers. OR decide on implicit dereferencing.


## Other Next Steps

Not entirely sure the appropriate order for these next steps, but they are important to get going.

Next Steps:
- Package system 
- Write a standard library
  - Note there will have to be a different version for every target architecture. Should be modular so there are as little platform specific parts as possible.
- Put in debugging symbols for typical debugging workflows
- Get some target platforms going
  - Rock-solid target for POSIX compliant systems
  - Web
  - IOS
  - Android
  - Windows Console
  - QR codes :P
  - Bare metal
    - Is the dynamic array feature even possible in this environment?
  - Desktop GUI
    - Windows (DirectX)
    - macOS (Metal)
    - Linux (OpenGL / Vulkan / ...)
- Implement inline assembly.
- Add multithreading.
- Hot-reloading.
- Write an optimization layer.
  - Done for the purpose of learning what optimized code is like, and how to write optimized code by-hand.

# EXTRA LANGUAGE FEATURES
- add the alloc keyword as an expression (kind of like var assignment/func decl).
- add the free keyword as a statement.
- \>> operator for piping function calls to object instances
    - I'll have it in the syntax parser as an expression, because '>>' is an operator.
- double variable initialiazation
    Ex) int last_x, last_y;
- "stringification" of enums.
  - For easy printing of things that have a type based on an enum.
- lambda functions.
- Complex numbers, 2i + 3
- 1_000_000_000 (underscores for better readability)
- make Quaternions a default type.

# LANGUAGE DESIGN DISCUSSION

Package System, Some Goals:
- If I have a file, I want to know what things the file depends on.
- If I have a project, I want to know all the dependencies on the project.
- Building another persons project should not require an installation of dependencies (like npm). ```npm install``` annoys me.
- Gonna use the keyword, "import".
- Single-header file libraries are really nice to work with.
- I like the idea of orderless functions. So no need for function decl if the order of definition at top-level scope changes.
- I like the idea of namespacing imports, so that depencies are explicit when used within the file.

Range based for-loops:
- there are common patterns where I want to change the index as it iterates, and I feel that this is odd when using a range based for-loop.
- there is also the case where the elements of the underlying array are modified as the loop is done. This seems odd with a 
rang-based loop too? Unless the iteration is with a reference variable?

implicit dereference / reference types:
- if you have this, then you might not know that you are dealing with a pointer. And this is not explicit, and I generally dislike a lack of explicitness.
- But then the advantage of implicit deref is not having to change large swaths of code if you wish to go from a non pointer type to a pointer type.

## THINGS I NEED/WOULD-LIKE TO LEARN
- Statis Single Assignment (SSA)
  - Basically we are treating each intermediate value of some variable as it's own variable.
  - https://en.wikipedia.org/wiki/Static_single_assignment_form
- Consider branching and Phi nodes for compiler optimization.
    - Branch prediction. What is this? How do CPUs do it?
- Check out what vectorizing is.
- Look into quantization (making ops smaller size)?
- What is a GPR?
