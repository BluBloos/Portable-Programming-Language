// Copyright (c) 2023, Noah Cabral. All rights reserved.
//
// This file is part of the Portable Programming Language compiler.
//
// See file LICENSE.md for full license details.


// This file is part of the first draft of the Portable Programming Language.
// This file is phase1, which is the first phase of the first draft.
// This file implements a full program, as do all phases.
// This is a full program for a console-based Snake game, adapted from https://github.com/x0st/snake.


// ------ VARIABLES, LITERALS, AND TYPES ------
unusedNamespace :: namespace {

    someVar : int = 0;      // a variable declaration.
    someVar2 : s64 = 0;     // type `int` is actually an alias for fundamental type `s64`.

    // integer fundamental types.
    
    b : u8;
    c : u16;    c2 : unsigned short;
    d : u32;
    e : u64;
    f : s8;
    g : s16;    g2 : short = ?;
    h : s32;
    // `?` is explicit undefined variables.
    // otherwise, variables are always zero initialized.

    // long is not supported.
    // there is no `const` keyword.
    // TODO: would we want 128 size support?

    // floating point fundamental types.
    i : float = 0.4;      i2 : float32 = 0.4;
    ii : double = 0.4;    ii2 : float64 = 0.4;

    // boolean fundamental type.
    j : bool = true;

    // pointers.
    k : ^int = @someVar; // address-of is `@`.
    ^k = 3;              // dereference is `^`.

    // TODO: consider not doing null-terminated strings.
    basicString : ^char = "Hello, world!";  // null-terminated 0-127 ASCII string literal.
    f : ^u8             = "Hello 🧠!";      // but, all strings literals are actually UTF-8.

    // type inference.
    someVar3  := 0;        // default type for a plain number is `int`.
    var4      := 0.f;      // suffix of `f` means `float`.
    var5      := 0.0;      // if the number is a decimal but there is no `f`, it is a double.
    var6      := "Hello";  // default type for a string literal is `^char`.
    var8      := true;     // `bool` type.
    var9      := 'a';      // `char` type.
    var11     := 0u;       // suffix of `u` converts `int` to `u64` type.
    var12     := var9;     // this uses the type of var9.

    // type inference does not work the other way around. i.e., a type cannot
    // be assigned to a literal using what the literal is set into.

    #if 0 { // TODO: add support into these.
        
        // potentially redesign these.
        var7      := "😂";     // if the string literal contains non-ASCII characters, it is UTF-8 encoded and type `^u8`.
        var10     := '👌';     // if the character literal contains non-ASCII character, it is UTF-8 encoded and u32 type.

        c : float = 1.0e9;              // scientific notation
        a : u8    = 0b01010101;         // binary literals
        b : u8    = 0xFF;               // Hexadecimal literals
        bigNumber := 1_000_000;         // underscores are ignored and are only for readability.

        // bitfields.
        a : int : 3; // how this works is that int:3 is a type. it is a modified int type sort of deal.
        b : int : 5;
        c : int : 4;
        d :: u64 : 3 = 4;
        a : short : 17; /* Illegal! */
        y : int   : 65; /* Illegal! */
        myBitfield : bool; // bool type is actually an alias for uint8_t : 1!

    }
}

// SYNTAX DETAIL:
// =======
//
// runtime variable declarations go as     `identifier : type = value;`.
// type inference omits the type and we get `identifier := value;`.
// variable declarations are `statement` grammar objects.
//
// we put the name before the type for readability.
// this way, you get to read what the meaning of the thing is first.
//
// the full program is just a list of statements.
// the program is describing the machine instructions for the CPU to execute.
// variable declarations at a global scope put their storage space in some sort of 
// region for such vars in .EXE, increasing .EXE size.
// runtime variable declarations either put it in a register or on the stack.


// ------ OPERATORS ------
sillyNamespace :: namespace {

    // TODO: operator overloading?
    #if 0 {
        A :: struct  {
            x : float;
        }

        B :: struct {
            w : double;
        }

        inline operator+ := (a:A, b:B) -> double {
            return a.x + b.w;
        }
        // ^ note that using the inline keyword here on the function
        // definition forces the compiler to inline this function
        // wherever it is called.

        a := A {1.f};
        b := B {2.0};
        z := a + b;
    }

    a : int;
    b : int;

    // binary operators.
    a + b;    // addition.
    a - b;    // subtraction.
    a * b;    // multiplication.
    a / b;    // division.
    a % b;    // modulus.
    a & b;    // bitwise and.
    a | b;    // bitwise or.
    a ^ b;    // bitwise xor.
    a << b;   // left shift.
    a >> b;   // right shift.
    a && b;   // logical and.
    a || b;   // logical or.
    a == b;   // equality.
    a != b;   // inequality.
    a < b;    // less than.
    a > b;    // greater than.
    a <= b;   // less than or equal to.
    a >= b;   // greater than or equal to.
    a = b;    // assignment.
    a += b;   // addition assignment.
    a -= b;   // subtraction assignment.
    a *= b;   // multiplication assignment.
    a /= b;   // division assignment.
    a %= b;   // modulus assignment.
    a &= b;   // bitwise and assignment.
    a |= b;   // bitwise or assignment.
    a ^= b;   // bitwise xor assignment.
    a <<= b;  // left shift assignment.
    a >>= b;  // right shift assignment.

    // unary operators.
    -a;  // negate.
    !a;  // logical not.
    ~a;  // bitwise not.
    ^a;  // pointer dereference. this op is well-defined in assignment_and_identifiers.c
    @a;  // address of.

    a ? b : c;  // ternary.

    // TODO:
    // I envision member access to just be an offset from @identifier = UID<T>,
    // but where T needs to have the member as verified by SEMA.
    // TODO: ^ just need to re-read the crap I wrote a while ago about this stuff.
    // there was a whole assignment_and_identifiers.c file. go through this.

    a.b;  // member access.
    // member access also works for what in C++ was :: and ->.
    // i.e. with namespace access it is also `.` and with member access through
    // pointer it is also `.`.

    a[b];  // subscript.
    a();   // function call.
    a++;   // post-increment.
    a--;   // post-decrement.
    ++a;   // pre-increment.
    --a;   // pre-decrement.

    #if 0 {
        // TODO: comma as an operator is a thing that needs to be looked into.
        a, b;  // comma.
    }

    // C operator precedence is the thing being used here:
    // https://en.cppreference.com/w/c/language/operator_precedence

}


// ------ COMPILE-TIME VARIABLES, NAMESPACES, AND IMPORTS ------
unusedNamespace2 :: namespace {
    SOME_VAR   :: 1;
    SOME_VAR_2 :: int = 1;
}

pal :: #import "ppl.pal"; // this marks the `ppl.pal` namespace as explicitly imported.
                          // and creates a new namespace `pal` that aliases `ppl.pal`.

plib :: #import "ppl.types";

#import "ppl.math"; // this is a thrust into the global namespace.

#if 0 {
    // TODO:
    #include "someFile.ppl" // this is the legacy include idea. a simple file-system based copy-pasta.

    // TODO:
    // there's an interesting idea here of automatically translating other languages into PPL AST.
    // for example some C99 compatibility.
    legacyMath :: #translate_c99 #include "math.h"
}

MAP_HEIGHT :: 15;
SNAKE_BODY :: '*';
MAP_WIDTH  :: 40;

// SYNTAX DETAIL:
// =======
//
// compile-time variables are variables whose storage duration is during
// the compilation meta-program.
// the syntax for such declarations goes as `identifier :: type = value;`.
// and in the type inference case we omit the `=` and the type to get `identifier :: value;`.
//
// and by the way, the idea behind the `#` syntax is like, any keyword that has `#` as a prefix
// indicates a compile-time operation. i.e. these ops are executed during the running of the
// compilation meta-program.
//
// namespaces:
// ===========
//
// in the case of the `unusedNamespace2` namespace declaration above, this is using the
// type inference compile-time variable syntax. the right hand side is a namespace literal,
// and therefore the type of the compile-time variable is a namespace type.
//
// i.e., the `{}` is decorated with the keyword `namespace` to make it so.
// so the formal syntax there is `type data_pack`. this gives us a typed data pack literal grammar object.
// in fact, all data packs are typed. if there is no type used, it is determined implicitly.
//
// variables with the namespace type MUST be compile-time variables.
//
// imports:
// ========
//
// all symbols are automatically exported (unless static) from a compilation unit / file.
// but no namespaced symbols are automatically imported unless explicitly declared as such.
//
// therefore, the compile-time #import statement type is used to explicitly import a namespace from another
// compilation unit / file.
//
// the result of the #import operation is a proper `expression` grammar object, and thus this construct
// can be used anywhere an expression can be used. the type of the resulting expression is a namespace type.


// ------ DATA PACKS AND ARRAYS ------
FOOD_SYMBOLS : []char = {'%'; '$'; '&'; '@'; '+'};

arrayPlayground :: namespace {

    a : [10 + 20]int; // the value in [] must be known at compile-time.
    c :: 100;
    d : [c]int;

    foo := ( a : []int ) -> u64 {
        return a.size; // array types carry size information.
    }

    pA : ^int = a; // decay to pointer works no problem but we lose size info.
    bG := a;       // but by default we copy the 'array' type. the array type is actually a view.
                   // so this is not a deep clone. just copy view by value.

    F : []int = {0 ; 1 ; 2 ; 3 ; 4 ; 5 ; 6 ; 7 ; 8 ; 9}; // we can use data packs to init arrays.

    // dynamic memory allocation is an operating system specific function.
    // therefore, there is no `new` keyword.
    // the best that we can do is have the `malloc` function as part of the standard lib.
    F := malloc(int, 10); // malloc first parameter is `Type`.
    defer free(F);

    // TODO: JAI has this idea of a context where a context has an assigned memory allocator.
    // maybe that is an interesting idea.

    // unless we do the memory context thing, dynamic arrays _cannot_ be a core part of the language.
    // they must be a part of the standard lib of the lang.
}

// SYNTAX DETAIL:
// =======
//
// anything in `{}` is a data pack. this is a proper grammar object.
// data packs contain a set of data delimited by `;`. the last item in the pack
// does not require `;`.
//
// they can be used:
//   - as a body of code (a set of statements)
//   - as an anonymous struct literal ( a set of variable decls )
//   - to initialize struct variables via a struct literal.
//   - namespace literals (a set of variable decls)
//
// when used to initialize a compile-time variable the `;` after the closing `}` is optional.
// the `;` is also optional when used as bodies with for/while/if/switch/function constructs.


// ------ FUNCTIONS ------
is_map_border := (x : int, y : int) -> bool {
    return (y == 0) || (x == 0) || (x == MAP_WIDTH) || (y == MAP_HEIGHT);
}

exit :: () {
    input_off();
    hTerm := pal.get_or_create_terminal();
    hTerm.print("\n\n###### THANK YOU FOR GAME ######\n\n");
}

// SYNTAX DETAIL:
// =======
//
// functions are first class citizens. first-class citizen means they can be the type of a variable.
// enabling passing them around, setting them, whatever you can think of.
//
// we can see that the type in `type data_pack` is a function type.
// so the above is defining a runtime variable `is_map_border` via the type inference syntax
// using the function literal on the right-hand side.
// function types have the parameters in `()` and the return type after `->`.
//
// the `exit` function returns void and takes no params. the `()` is required otherwise
// it would be ambiguous if what we are defining is a function body or an array literal.
// hence, when a data pack has no type, it is an array literal.
//
// every function ever has its code static in the .exe.
// and thus runtime variables that have the function type are simply a pointer to the function code.
//
// now, this kind of model could incur some performance penalties.
// the function would be called through a pointer stored in a variable, which is not ideal.
// to this end, variables in PPL are "compile-time until proven otherwise".
// and for a compile-time function, PPL knows it can inline the function address at the call site.
//
// also note that there are a few variable captures going on. captures don't require any special
// syntax, and by default do so by capturing the storage location of the variable.
// and consequentially, they have a few limitations to prevent dangerous situations.
// essentially, a variable can only be captured if it is static storage duration,
// or if its storage duration is less than or equal to that of the function.
//
// and within the `exit` function we can capture a function that has been defined later in the global
// scope. this is perfectly fine.


// ------ SEMANTIC MACROS ------
#if 0 {
    // TODO:
    MAP_WIDTH  :: Code<int> = #save_code 40;

    exit :: Code<()->void> = #save_code {
        // backtick defers lookup of the stuff in the AST that we are saving.
        `input_off();
        hTerm := pal.get_or_create_terminal();
        hTerm.print("\n\n###### THANK YOU FOR GAME ######\n\n");
    }
}

// SYNTAX DETAIL:
// =======
//
// These are semantic macros. They are shamelessly ripped-off from JAI.
// A macro is this saved chunk of AST that can be inserted/inlined later.
// This allows creating functions / constants / things that are always inlined.
//
// the #save_code runs at compile time to grab the thing and save the AST of that.
// then when later evaluating MAP_WIDTH as a value, we can insert the AST there to get the value.


// ------ STRUCTS ------
SnakeBody :: struct {
    // TODO: add ideas for how to pack a struct.

    x : int;
    y : int = 0; // default values used for zero-initialization.

    static make_default := () -> SnakeBody
    {
        // the static keyword goes on the left of the name because the identified thing is static.
        // the type is not static. these modifier ideas always modify the thing to the right.
        return {-10; -10};
    }

    #if 0 {
        // TODO: maybe this is an interesting syntax for one-liners.
        static make_default := return SnakeBody {-10; -10};
    }
};
// TODO: Add some sort of syntax to denote that structures should be packed
// and ignore that alignment stuff.

// SYNTAX DETAIL:
// =======
//
// the above declares a compile-time variable called `SnakeBody` that has the `struct` type.
// The above could be written as `SnakeBody :: struct = struct {}`.
// 
// basically, the idea is that such instances of compile-time vars with `struct` type
// create a new type for us with the same schema as the compile-time var.
//
// structs are simply a group of data that exists in memory. In C++ parlance, this is POD (plain old data).
//
// for member functions of structs, these are no different to any other function-typed variable
// in the language. there is thus no vtable and no runtime polymorphism. the struct stores the function
// pointers, or if the member function is a compile-time var, the function address is inlined and there is
// no data member in the struct for the function pointer.
//
// also, do not be deceived. this is not a class. there is no such thing as `private`, `public`, etc.


// ----- IMPLICIT . SYNTAX ------

not_called_function :: () {

    B := struct {
        a : int;
        b : float;
    }
    // ^ this isn't a compile-time struct variable, so it doesn't define a type.

    // what type is B?
    if struct == type_of(B) then print("yes, this is true");

    // thus when using data pack assignment we must do the below:
    B = struct { 1; 1.f };

    // or we could of course just assign to the members:
    B.a = 1; B.b = 1.f;

    a := 10;

    // the main use-case for Implicit . syntax is within data packs like below:
    B = { .a = a; .b = 1.2f };
    // this re-slots the logical data pack member to another logical slot.

    // but we can also use it here, like so:
    someBitFlag := MyEnum.Flag1 | .Flag2 | .Flag3;
    // this kind of implicit idea cannot be combined with the re-slotting idea
    // above. it is an either or, and the one used is based on the context.
    //
    // this second kind of implicit . idea can only be done when the expression has just a single member access.
    // otherwise the implicit . wont know to what thing the member access is on.

    // the below is not allowed. designated initializers to array slots must be in order
    // WHEN USED WITH non-designated initializers.
    A : [10]u32 = { .3 = 1; .0 = 2; 3; }; // gives compiler-error.

    // gives an array of [ 2; 3; 0; 1; 0; 0; 0; 0; 0; 0 ].
    // remember, everything is zero initialized unless explicitly marked as uninitialized.
    a2 : [10]u32 = { .0 = 2; 3; .3 = 1; };
}


// ------ ENUMS ------
Directions :: enum
{
    LEFT = 1;
    RIGHT;
    UP;
    DOWN;
}
// TODO: for right now, member functions in enum data packs are not supported. but
// is this possible and is it valuable?

// SYNTAX DETAIL:
// =======
//
// An enum is an an idea to declare a bunch of namespaced constants efficiently.
// The rules are:
//   - All variables are type u64.
//   - We can also do this pattern where any subsequent member var is +1 the value of the previous
//     member var if not explicitly set.
//
// recall the syntax of `type data_pack`. So if we are going as `enum {}`, enum is a legit type.
// we could actually write the above as `Directions :: enum = enum {}`. that would be stupid, but it works.
//
// this means that the variable `Directions` has a type of `enum`.
//
// the idea is that such instances of compile-time variables create a new type for us.
// var of this type can be just one of the members declared in the compile-time var.
//
// but not only do they define a new type, but `enum` variables actually have a concrete data layout
// in memory. is is effectively like a structure with each member being those value options declared
// in the enum.
//
// thus, if we do an access like `Directions.LEFT`, this is a member read from the compile-time
// variable `Directions`. Since all this stuff is constant at runtime, the compile-time variable
// does not need to be stored and the enum values can be inlined at the site where they would have
// otherwise been a member access.


// ------ GENERICS ------
snake : plib.Array<SnakeBody>;
// the ppl.types namespace implements a generic array type.
// it operates very simply, just like stb stretchy buffers.
// different versions are compiled based on the uses of the generic type.

// TODO: should there be support for templates that are by value?
// I am a betting man, and I bet things would be simpler if templates were by type.


// ------ FOR LOOPS ------
init_snake := () {
    // the `for` syntax takes a set to iterate over.
    // here we use the set construction syntax `..` to build a set of integers (assumed u64).
    // the loop below will go for values 0 through 4 inclusive.
    for 0 .. 5
    {
        body : SnakeBody = SnakeBody.make_default();
        snake.push(body);
    }

    snake[0] = SnakeBody {5; 3};
}

move_snake := ()
{
    switch direction {
        case Directions.DOWN:  snake[0].y++;
        case Directions.LEFT:  snake[0].x--;
        case Directions.UP:    snake[0].y--;
        case Directions.RIGHT: snake[0].x++;
    }

    for 1 .. snake.size
    {
        // within the loop, there is an implicit value `it`. this is the current value in the set.
        // there is also `it_index` which is the index of the current value in the set.
        snake[it] = snake[it - 1];
    }
}

snake_ate_food := () -> bool {
    // Array<> as well as the static array [] type are sets.
    // therefore, we can loop over them.
    for snake {
        if (it.x == foodX) && (it.y == foodY) then return false;
    }
    return false;
}

// the code below is useless but is here to further explain the for/while loop ideas.
this_func_is_not_called := ()
{
    #if 0 {
        while i : int = 0; i < 10; i++
        { // the old for loop syntax is still available but we use `while` instead.
            j : int;
            while j < 10; j++
            { // and we can use just two things: continue expression and loop postamble.
                k : int;
                while k < 10
                { // and we can use just one thing: continue expression.
                    k++;                
                }
            }
        }
    } // TODO: All these different loop syntaxes are a little annoying from the
    // "oh I actually have to support this on the grammar side". so for now let's
    // just defer implementing these and stick with the continue expression loop syntax.

    for 0 .. 5 {
        for 0 .. 10 {
            // the second `it` within this loop shadow the first `it`.
        }
    }

    // so we introduce a new for loop syntax to allow for override the implicit `it`.
    for i in 0 .. 5
    {
        // `it_index` is still available.
        // so we also allow for override that too.
        for j, j_idx in 0 .. 6
        {
            #if 0 {
                // there is also a use-case where we can exit the outermost loop with this
                // sort of parameterized break statement.
                break i;
            }
        }
    }


    #if 0 { 

        // data packs with no type to the left of them can be inferred as array literal type.
        a := {0; 1; 2; 3; 4; 5; 6; 7; 8; 9};

        // Can I have a runtime variable be equal to a set of integers?
        z : Span = 0 .. 3;
        z = Span {0; 3}; // a span is just actually just a struct of two integers.

        b : []int = a[ z ]; // if take a "slice" of an array, I just get back another array.
                            // this is because all arrays are actually "array views".

        for e, i in b; // this is a silly loop that does nothing :P
    }

    // TODO:
    // need to use more ideas from the JAI:
    //   - remove and insert into set as iterate over it.
    //   - iterate over the set backwards.
    //   - the idea that for loops are just a macro thing.
}


// ------ TYPES and ENUM STRINGS ------
typeExample :: () {
    varC := SnakeBody {};
    varA : Type = type_of(varC);
    varB := bool;

    if varA == varB {
        // we won't ever get here.
    }

    varA.name; // gives "s64" as "int" is an alias of that.

    info : TypeInfo = type_info(varA);
    for info.members
    {
        assert(type_of(it) == TypeInfoMember);
        it.offset;
        it.name;
        it.type;
    }

    // instead of iterate we can also just use the members directly like so:
    #assert offset_of(SnakeBody.y) == size_of(SnakeBody.x); // size_of just "does" the conversion
                                                            // if needed using type_of().
    // offset_of is to be implemented as this fun macro idea that takes in an AST structure.
}

debug_print_directions := ()
{
    hTerm := pal.get_or_create_terminal();

    // specific enum type values are just one elem,
    // but the type info for a specific enum type tells us about all the possible members.
    for type_info(Directions).members
    {
        hTerm.print("Directions.%s = %d", it.name, idx);
    }
}

enum_stringify :: (dir : Directions) -> ^u8
{
    info := type_info(Directions);

    // TODO: info.valueSet? for enums that's OK, but other types
    // can be exponentially large for all possible values so that's dumb.
    for info.members {
        // TODO: this seems like the best that I can do ...
        memberValue := cast<^u8>(@Directions) + it.offset;
        if (memberValue != dir) continue;
        return it.name; // the .name in any type info is baked into .exe
    }
}

// SYNTAX DETAIL:
// =======
//
// Note that any compiler built-in along with any function as part of the standard library
// are snake case.
//
// types are first class citizens. they can be stored in variables.
// the compilation meta-program keeps track of all the types.
// any variable with type `Type` can be set to any of the tracked types.
//
// `type_info` is a compiler built-in that can get useful info about a tracked type.
// `type_of` is a compiler built-in that can get the type of a value.
//
// enum stringify:
// ===============
//
// so the enum stringify stuff and what that implies is up for debate right now.
//
// but it seems like it should be OK. the "up for debate part" is taking the address of a type.
//
// basically you have this compile-time var with type `enum` to declare your type, this is known.
// or maybe the compile-time var with type `struct` to declare some struct type.
//
// so in the compilation meta-program, these variables of course actually exist. the data layout
// of the `struct` var is the schema that it is defining, no surprises there. the enum data layout has
// been explained before and within the ENUM section.
// the actual data within the layout are the default members that would be used to default initialize a runtime
// version of these things.
//
// since these are compile-time variables, they won't actually exist at runtime.
// e.g. the enum-struct thing will be used during compilation to inline things.
// 
// given all this, what does it mean to take the address of (which is a runtime operation) of these
// compile-time variables?
//
// it's actually not so terribly complicated. basically, these compile-time variables will be forced to persist
// at runtime. they will be stored in a read-only section of the .exe.
// hence, the address to return is where the thing is in that readonly section.
//
// one last final thing to mention would be that for structures, the only way to access the compile-time variable version
// would be via the address thing. OR, if that member variable was a static member. static storages of members
// would actually use this concept too.


score : int = 0;
foodX : int = 0;
foodY : int = 0;
direction : int = Directions.DOWN;
foodSymbolNum : int = 0;


main := ( argc : int, argv : ^^char ) -> u32
{
    debug_print_directions();

    // functions do not require forward declarations and can therefore
    // be called from at this point in the file, despite being defined later in
    // the file.
    input_on();

// ------ DEFER STATEMENTS ------
    defer {
        input_off();
        // break statements exit the data block, not just loops.
        break;
        hTerm.print("this will not print");
    }

// SYNTAX DETAIL:
// =======
//
// the `defer` keyword is taken from Go, and the many other languages that have adopted it.
// this keyword takes a statement on the right-hand side and defers the execution of said
// statement until the end of the current scope.
//
// this is implemented as a sort of AST yank so if the statement defines a variable,
// that variable is not visible ever because it won't be defined until the end of the scope.


    generate_food_coord();
    generate_food_symbol();
    init_snake();

    hTerm = pal.get_or_create_terminal();

    // no `()` ever for if/for/switch... statements.
    while true
    {

        if hTerm.wait_until_input_ready()
        {

// ------ SWITCH STATEMENTS ------
            // switch statements require explicit fall-throughs.
            switch hTerm.getchar() {
                case 'D': fall;
                case 'd':
                    if direction != Directions.LEFT then direction = Directions.RIGHT;
                case 'S': fall;
                case 's':
                    if direction != Directions.UP then direction = Directions.DOWN;
                case 'A': fall;
                case 'a':
                    if direction != Directions.RIGHT then direction = Directions.LEFT;
                case 'W': fall;
                case 'w':
                    if direction != Directions.DOWN then direction = Directions.UP;
                case 27: // ESC key
                    exit();
            }
        }

        #inline move_snake();
        // #inline is a compiler op that takes the function call on the right and inlines
        // it. it is `#` because this is formally a compile-time operation. it's kind of like
        // a macro. it's actually the same thing. except we specify it from the other side.

        if snake_ate_food()
        {
            generate_food_coord();
            generate_food_symbol();
            snake.push(snake[snake.size - 1]);
            score += 7;
        }

        if is_out_of_border() || snake_eats_itself() then exit();

        draw();
        pal.sleep(100);

    }

    return 0;
}

input_on :: ()
{
    hTerm := pal.get_or_create_terminal();

    currTermAttrs := hTerm.get_attributes();

    // TODO: Does this terminal PAL stuff work for platforms other than Unix?
    tAttrFlagType = pal.TerminalFlag;
    currTermAttr.flags &= ~( tAttrFlagType.INPUT_CANONICAL | tAttrFlagType.ENABLE_ECHO );
    currTermAttrs.special_characters[pal.TerminalSpecialChar.VTIME] = 0; // timeout for non-canonical read.
    currTermAttrs.special_characters[pal.TerminalSpecialChar.VMIN]  = 1; // minimum number of characters for
                                                                         // non-canonical read.

    hTerm.set_attributes(currTermAttrs);
}

generate_food_coord :: ()
{
    plib.seed_rand( pal.time() );
    // If you do not supply max to rand(), just returns maximum integer. Always returns unsigned.
    foodX = 3 + plib.rand() % (MAP_WIDTH - 3);
    foodY = 3 + plib.rand() % (MAP_HEIGHT - 3);
}

generate_food_symbol := ()
{
    foodSymbolNum = plib.rand() % 5;
}

snake_eats_itself := () -> bool

    #if 0 {         // TODO: double variable init.
        // we are using the `,` comma syntax here which creates a grammar construct called
        // an "expression pack". this has no runtime representation and is used to communicate
        // "routing" behavior.
        head_x, head_y := snake[0].x, snake[0].y;
        // TODO: we could also look into multiple function returns.
    }

    head_x := snake[0].x;
    head_y := snake[0].y;

    for snake {
        // this is a single-line `if` statement.
        // parenthesis are never required for if statements.
        // and actually will not work whatsoever.
        if (it.x == head_x) && (it.y == head_y) then return true;
    }

    return false;
}

draw := ()
{
    hTerm := pal.get_or_create_terminal();
    hTerm.run("clear"); // synchronous.

    for i in 0 .. MAP_HEIGHT + 1
    {     // TODO: should Spans be inclusive or exclusive?
        for j in 0 .. MAP_WIDTH + 1
        {
            if is_map_border(j, i) {
                hTerm.print("#");
            } else if is_snake_body(j, i) {
                hTerm.print("%c", SNAKE_BODY);
            } else if is_food(j, i) {

                // note the only reason we do this explicit indexing here is to demonstrate two things:
                //   - decay to pointer.
                //   - the size_of built-in. 
                symbol : char = ^( cast<^u8>(FOOD_SYMBOLS) + size_of(char) * foodSymbolNum );
                // also note that with the casting idea this is when we want to make something be considered
                // another type. however this will not help compiler error messages that warn about truncation.
                { // there is a special built-in for this.
                    a:u32=1;
                    b:u16:2;
                    b = trunc(a); // this will error about truncation so need to use trunc().
                } 

                hTerm.print("%c", symbol);
            } else {
                hTerm.print(" ");
            }
        }

        hTerm.print("\n");
    }

    hTerm.print("\n###### score: %d ######\n\n", score);
}

is_out_of_border := () -> bool
{
    return (snake[0].x == 0) || (snake[0].y == 0) || (snake[0].x == MAP_WIDTH) || (snake[0].y == MAP_HEIGHT);
}

is_snake_body := (x : int, y : int) -> bool {
    for snake {
        if (it.y == y) && (it.x == x) then true;
    }
    return false;
}

is_food := (x : int, y : int) -> bool {
    return (foodX == x) && (foodY == y);
}

input_off := ()
{
    hTerm := pal.get_or_create_terminal();
    hTerm.reset_attributes();
}


// not part of the snake thing but here as demo.
#if 0 {
    // TODO:
    duffs_device :: () {
        to : ^short; 
        from : ^short;
        count : u32;
        {
            n : u32 = (count + 7) / 8;
            switch count % 8 {
                case 0: do { ^to = ^from++; fall;
                case 7:      ^to = ^from++; fall;
                case 6:      ^to = ^from++; fall;
                case 5:      ^to = ^from++; fall;
                case 4:      ^to = ^from++; fall;
                case 3:      ^to = ^from++; fall;
                case 2:      ^to = ^from++; fall;
                case 1:      ^to = ^from++;
                        } while --n > 0;
            }
        }
    }
}

// SYNTAX:
// =======
//
// The thing to note about the example above is that the case labels are not required to
// be directly descendant to the switch statement in the AST. they can be anywhere pretty much
// and just denote a location to jump to.

