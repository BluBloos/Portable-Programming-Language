The task is as such. We are to read the LLVM IR. Get inspired. And invent our own, more slim IR.

Of course, there are probably benefits to using IRs that other people have used.

So maybe we are not inventing our own IR, but rather chosing some small subset of the LLVM IR.

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

- void
- float
- double
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

