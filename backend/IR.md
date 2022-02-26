# Roadmap

What is the path forward for developing the underlying IR for PPL? 

- We are to first write a Hello, World! example for each underyling target platform of the MVP. Each example should be easily built and tested to run on each platform.
  - EDIT: We have done some good work for the iOS platform. But the fact remains that the base level API for this platform exposes at its lowest level only objective C. Do we are left with complexities in order to make developing for this platform using PPL a sensible experience.
  - Thus, we exclude iOS from the set of targets that will be working for the first pass of the IR. Of course, we will still include iOS in the MVP, but it remains a challenge to get working for a first pass IR.

- Using the low-level knowledge of each platform, invent a higher-level textual language for representing the IR. In this step, we are likely to leverage the existing and well-documented LLVM IR. Our IR should have a corresponding in-memory representation, and the text files can be converted to said in-memory representation.

- Then we are to write just one Hello, World! example using our newly invented IR.

- Finally, we are to write the corresponding translation code for each mapping from the in-memory IR to the underlying target platform executable binary.
  - We note that this step will also include the creation of an extremely minimal implementation of the PPL standard library, which as of the moment includes one function, print.

# Assembly IR (Not LLVM IR compatible)

We want the intermediate representation to have both a bytecode and a textual form. This just makes life super easy. So to make my life even easier than easy, we want to make the definition of the assembly language be one-liners. Every line is a statement.

Then, converting from textual form to bytecode works like so:
- Read each line of file
- For each line of file, do a bunch of if-statements, maybe a few switch statements here and there.
- Done.

## Directives

.org (where relative to start of file is this code)? \
.section (define section information for preceding lines)

## Comments

Delimited by ';' and go until the end of the line

## Identifiers

NOTE: Do we need global identifiers?

@ - prefix for global identifier \
% - prefix for a local identifier

This is the regex for valid identifiers \
"[%@][-a-zA-Z$._][-a-zA-Z$._0-9]*"

## Opcodes

- add
- sub
- bitcast
- ret
- mul
- div
- call
- store
- load

## Primitive Type Names

TODO(Noah): Add x86_amx and x86_mmx

- void
- float
- double
- float128
- uint64
- uint32
- uint16
- uint8 
- int64
- int32
- int16
- int8

## Calling Conventions

- ccc (standard C calling convention)
- fastcc (Fast calling convention)
  - Pass things in registers
- coldcc (Cold calling convention)
  - Used for calling function that don't get called often.

## Threading

For now we ignore this. Add later.

## Functions

For linking: \
using __________

Defining an internal, callable function: \
define _________

## Constant Data

@a = int32 5 \
OR \
@b = int32 [5,6,7,8,3]

