// C operator precedence, for reference
// # https://en.cppreference.com/w/c/language/operator_precedence

// NOTE(Noah): Apparently everything good about language design has already been done...
// So what if we made PPL flexible? -> metaprogramming?

// NOTE(Noah): Currently considering range based for-loops.
    // because there are common patterns where I want to change the index as it iterates.

// NOTE(Noah): Currently considering implicit dereference.
    // because if you have this, then you might not know that you are dealing with a pointer.
    // this is not explicit, and I generally dislike a lack of explicitness.
    // NOTE(Noah): But what if there was some other feedback mechanism?
        // Like it syntax highlighted your pointer variable?    

/* TODO

First Class Priority Tasks:

    - Port Compiler codebase to C/C++.
        - parsing compound_ops misses the last 8 char (I reckon because there is no whitespace after)
        - 3.c parses as a symbol. Should this be the case?
        - \n go through as just n. 
        - bad.c parses, returning no tokens. Is this valid?
        - any / are just not getting parsed. (for example in #inlude <ppl/console>)
        - The parser understands that files may be in utf8.
            - Leads to any utf8 char literals being parsed as symbols.  

    - Is the language in a usable state? Look at C89 spec and your own projects.
    - The answer to this question is a "feel" thing.
        http://port70.net/~nsz/c/c89/c89-draft.html#2.
        - Verify that names of functions are legal (cannot begin with $ for example)
        - Add all desired variable types and sizes Ex) 
            - char (a unicode coidepoint). 
            - uint64 (unsigned 64 bit integer).
            - float (32 bit floating point).
            - double (64 bit floating point).
        - Add bitshift operators. Ex) >> and <<.
        - Ensure all postfix and prefix are there Ex) ++X or X++
        - Add -> for dereffing pointers.

    - Port C backend to ANSIC
    - Add LLVM IR as backend

- Read the Rust langauge specification to learn how they make memory safe.
- Package management

- add the alloc keyword as an expression (kind of like var assignment/func decl).
- add the free keyword as a statement.
- >> operator for piping function calls to object instances
    - I'll have it in the syntax parser as an expression, because '>>' is an operator.
- double variable initialiazation
    Ex) int last_x, last_y;
- add single character printing of unicode code points.
- "stringification" of enums.
- Can define structs/functions anywhere. Like: inside another function if wanted.
- vector and matrix math ops are first class citizens of the language.
- lambda functions.
- map/dict type (hash table is first class)
- 0b01010101 (binary literals)
- 0xFF (Hexadecimal literals)
- Complex numbers, 2i + 3
- 1.0e9 (scientific notation)
- 1_000_000_000 (underscores for better readability)
- 64 bit floating point numbers
- struct literal
v = Vector3{1, 4, 9}
- make Quaternions a default type.

Check out the things below,
- Consider branching and Phi nodes for compiler optimization.
- Check out what vectorizin is.
- Look into quantization (making ops smaller size)?
- What is a GPR?
*/

/* CONDENSED/TRIMMED C89 SPEC

When working with a translation unit:
- tokenizing the file
- execute preprocessor directives, expand macros
- escape sequences in string literals and character literals are converted

keywords:
auto, double, int, struct, break, else, long, switch, case, enum, register, typedef
char, extern, return, union
const, float, short, unsigned
continue, for, signed, void
default, goto, sizeof, volatile
do, if, static, while

identifiers (like a function name or variable name):
_ a b c d e f g h i j k l m
n o p q r s t u v w x y z
A B C D E F G H I J K L M
N O P Q R S T U V W X Y Z
0123456789
    - first character cannot be a digit

types:
    - Types in C89 are ODD. I do not care... will implement my own.

suffixes for literals
    - I do not give a shit about these. I think they are obnoxious. None of these please.

*/