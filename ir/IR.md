## 2022.02.03

The task is as such. We are to read the LLVM IR. Get inspired. And invent our own, more slim IR.

Of course, there are probably benefits to using IRs that other people have used.

So maybe we are not inventing our own IR, but rather chosing some small subset of the LLVM IR.

## 2022.02.04

It's a hard task that I have given myself. Frankly, as I read over the LLVM IR language specification, this thing is HUGE. I find it's almost a little like C. For example, there is an array type, and this is a first-class citizen...

My big question is then the following. At what level of abstraction are the underlying bytecodes for each machine architecutre?

Recall the architectures: x86, ARM, Davlik, WebAssembly

We know that x86 and ARM are going to be your typical assembly language. So it would be difficult to make anything lower level.

Dalvik? 

Hold on.

I see what the problem is here. The reason I cannot design a compelling or reasonable intermediate representation is because I have know idea what it is supposed to look like on the other end. What I need to do is manually write a hello-world routine for each of these things. I need to start to write by hand what I am going to have the compiler do.

We start with x86.

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

