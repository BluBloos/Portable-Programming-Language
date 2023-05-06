// Copyright (c) 2023, Noah Cabral. All rights reserved.
//
// This file is part of the Portable Programming Language compiler.
//
// See file LICENSE.md for full license details.


// This file is part of the first draft of the Portable Programming Language.
// This file is phase1, which is the first phase of the first draft.
// This file implements a full program, as do all phases.
// This is a full program for a console-based Snake game, adapted from https://github.com/x0st/snake.


// PPL considers a systems programming language as a set of statements where each is a 1:1 on top of the machine code.
// This isn't a strict 1:1 mapping but a set of language constructs that map to lean underlying structures.
// Aside, such a language is to be understood within the context of the compilation metaprogram.
// Statements in the language derive meaning from the current context as provided by this metaprogram.

// One of the most common ideas is to declare a variable.
someVar : int;      // a variable declaration.
// a var decl is a directive to the compilation metaprogram (detail explained within assignment_and_identifiers.c).
// this may result in an instruction to allocate stack space; otherwise, this updates the state of the metaprogram.
// this specific variable decl is at global scope, indicating a static storage duration.


// RUNTIME VARIABLE DECLARATION SYNTAX:
// ====================================
//
// identifier : type;
//  ^ this is a runtime variable declaration.
//
// identifier : type = value;
//                      ^ we can optionally initialize the variable.
//
// identifier := value;
//  ^ we can infer the type.
//
// variable declarations are `statement` grammar objects.
// we put the name before the type for readability. this way, you get to read what the meaning of the thing is first.


// FUNDAMENTAL TYPES:
// ==================
a00 : bool;
a0  : unsigned int;
a1  : u8;
a2  : u16;
a3  : u32;
a4  : u64;
//
a5  : char;  // 1 byte.
a6  : short; // 2 bytes.
a7  : int;   // 8 bytes.
a8  : s8;
a9  : s16;
a10 : s32;
a11 : s64;
//
a12 : float;
a13 : double;
a14 : f32;
a15 : f64;


// TYPE INFERENCE:
// ===============
//
// In PPL, every literal is strongly typed. This is what makes type inference possible.
//
a16 := 0;         // this is an int.
a17 := 0.0f;      // this is a float.
a18 := 0.0;       // this is a double.
a19 := 'a';       // this is a char.
a20 := 0u;        // this is an unsigned int.
a21 := true;      // this is a bool.
a22 := a21;       // also a bool. uses the type of a21.


// DEFAULT INITIALIZATION:
// ======================
//
// In PPL, any data is always initialized to the "zero" value for the type.
b : int = ?; // explicit undefined.
// however, we can explicitly leave something as undefined. this does the C behavior of leaving it uninitialized.
//
// note that `?` is an exception to the rule that all literals are strongly typed.
b2 := ?;     // compiler error. `?` is untyped.


// POINTERS:
// =========
//
a27 : ^int = @7; // this is a pointer to an int. @ is the address-of operator.
^a27 = 3;        // ^ is the dereference operator.
// these operators are explained in greater detail within assignment_and_identifiers.c


// OPERATORS:
// ==========
//
// For the ops below, C operator precedence is the thing being used here: https://en.cppreference.com/w/c/language/operator_precedence.
//
a : int;
b : int;
//
a + b;    // addition.
a - b;    // subtraction.
a * b;    // multiplication.
a / b;    // division.
a % b;    // modulus.
a & b;    // bitwise AND.
a | b;    // bitwise OR.
a ^ b;    // bitwise XOR.
a < b;    // less than.
a > b;    // greater than.
a = b;    // assignment.
a, b;     // comma operator.
//
a << b;   // left shift.
a >> b;   // right shift.
a && b;   // logical AND.
a || b;   // logical OR.
a == b;   // equality.
a != b;   // inequality.
a <= b;   // less than or equal to.
a >= b;   // greater than or equal to.
a += b;   // addition assignment.
a -= b;   // subtraction assignment.
a *= b;   // multiplication assignment.
a /= b;   // division assignment.
a %= b;   // modulus assignment.
a &= b;   // bitwise AND assignment.
a |= b;   // bitwise OR assignment.
a ^= b;   // bitwise XOR assignment.
//
a <<= b;  // left shift assignment.
a >>= b;  // right shift assignment.
//
-a;  // negate.
!a;  // logical not.
~a;  // bitwise not.
@a;  // address of.
^a;  // pointer dereference.
//
a ? b : c;  // ternary.
//
a.b;  // member access. member access also works for what in C++ was :: and ->.
//
a[b];  // subscript.
a();   // function call.
a++;   // post-increment.
a--;   // post-decrement.
++a;   // pre-increment.
--a;   // pre-decrement.


// COMPILE-TIME VARIABLES:
// =======================
//
// These are variables that are runtime variables from the perspective of the compilation metaprogram. When we are compiling, they get created.
// At .exe runtime, these are placed into a read only section of the executable based on what they were after the compilation metaprogram completes.
// But if unused, they are optimized out.
//
SOME_VAR   :: 1;
SOME_VAR_2 :: int = 1;
//
// syntax:
// =======
//
// identifier :: value;
//  ^ this is a compile-time variable declaration.
//
// identifier :: type = value;
//                ^ we can optionally include the type (type inference).
//
// compile-time variable declarations are `statement` grammar objects.


// STRUCTS AND DATA PACKS:
// =======================
//
Vector :: struct {   // declare compile-time variable `Vector`.
    x : float = 0.f; // default values are those that are used for zero-initialization.
    y : float = 0.f;
    z :       = 0.f;
};
//
// data packs:
// ===========
//
// anything in `{}` is a data pack. this is a proper grammar object.
// data packs contain a set of statement grammar objects.
//
// when used to initialize a compile-time variable, the `;` after the closing `}` is optional.
// the `;` is also optional when used as bodies with for/while/if/switch/function constructs.
//
// data packs are literals and are therefore strongly typed. the grammar construction goes as : `type data_pack`
// however,
// the "untyped data pack" is also a valid type, and is when the data pack does not have a type on the left.
// and this can be trivially cast to many sorts of data pack types.
//
// so we can have the untyped data pack:
{ // a scope.
    a : int;
}
// because then it makes sense anywhere where we want to put it as a code statement (like an if, while, for, etc).
// 
// structs:
// ========
//
// when we declare a compile-time variable of a `struct` type, by using a struct data pack, this registers a new type.
// the set of compile-time struct typed variables form the registry of user-defined types.
//
// the data layout for `struct` types contains information that describes a structure.
// later we introduce a type of TypeInfo. struct types are a sort of alias for that.
// the compiler will also arbitrate a .UID into the struct data layout so that it can be used as a type.
//
// thus, the identifier `Vector` can later be used in places where a type grammar object is expected.
//
// another thing the compiler does is put the actual instance members (x, y, z) into the compile-time `struct` variable.
// this storage is used if e.g. one of the members in the struct description is declared static.
//
// Once we create an instance of the described structure, we simply get a group of data that exists in memory.
// In C++ parlance, this is POD (plain old data).
// 
myVector := Vector { 2.f; 3.6f };
//
// here, we type the data pack with `Vector`, making this data pack encode the data of a `Vector` object.
// each statement-exp in the data pack wires into the member variables of the struct, in the order they were defined in the struct.
// since we only provided values for the first two members, the third one is zero initialized.
//
myVector = {};
myVector = null;
//
// the empty, untyped data pack is a special kind of data pack. this is explicit zero initialization. the power of this is in setting an already
// declared variable. this is an untyped literal and therefore an exception to the rule that all literals are strongly typed.
//
// null is an alias for the same idea as the empty data pack. explicit zero initialization.

// NAMESPACES:
// ===========
//
theBoys :: namespace { // declare compile-time variable `theBoys`.
    bro1 :: 1;
    bro2 :: 2;
}
//
// here we declare a compile-time variable with type of a namespace data pack.
// variables with the namespace type MUST be compile-time variables.
//
// a namespace compile-time variable is used by the compilation metaprogram to help organize spaces of names - no surprises here.


// IMPORTS and COMPILE-TIME STATEMENTS:
// ====================================
//
// all symbols are automatically exported (unless static) from a compilation unit / file.
// but no namespaced symbols are automatically imported unless explicitly declared as such.
//
#import "ppl.math"; // so we can mark a namespace as explicitly imported with the #import statement.
//
// ^ this above statement will thrust stuff from the `ppl.math` namespace into the current namespace.
//
// the #import statement is a legit statement and therefore has a return value. it returns a value typed as namespace data pack.
// 
pal :: #import "ppl.pal"; // therefore we can use the expression result as the value in the creation of a new variable.
// ^ this idea creates a new namespace `pal` that aliases `ppl.pal`.
plib :: #import "ppl.types";
//
// the `#` idea shows up when we are dealing with compile-time things.
// `#import` is specifically a statement that gets executed at compile-time by the compilation meta-program.


// ARRAYS:
// =======
//
FOOD_SYMBOLS :: []char = {'%'; '$'; '&'; '@'; '+'};
//
// the built-in array type is for static arrays and therefore the amount of elements within is constant.
// all array types under-the-hood are structs. they contain a `size : u64` member and a pointer to the first element.
// this makes all arrays "views".
//
arrView := FOOD_SYMBOLS; // so we can do something like this,
//
// and it will not be any sort of deep clone but rather a trivial copy of those two members.
// this means that array types can be passed to functions and retain their size information.
//


// FUNCTIONS:
// ==========
//
MAP_HEIGHT :: 15
SNAKE_BODY :: '*'
MAP_WIDTH  :: 40
//

is_map_border :: (x : int, y : int) -> bool {
    return (y == 0) || (x == 0) || (x == MAP_WIDTH) || (y == MAP_HEIGHT);
}

//
exit :: () {
    input_off(); // declarations at the global program scope do not need to be in order.
    hTerm := pal.get_or_create_terminal();
    hTerm.print("\n\n###### THANK YOU FOR GAME ######\n\n");
}
//
// SYNTAX DETAIL:
// =======
//
// functions are first class citizens which means that there is a function type and variables can be of that type.
//
// (x : int, y : int) -> bool
//                             ^ this guy is sitting in the grammar where a type is expected.
//                               indeed, this construction is the grammar for a function type.
//                               this particular function takes in two integer parameters and returns a boolean.
// 
// the grammar for a function type goes as first `(`, then a `,` delimited list of variable declarations, closing with `)`,
// and optionally a return type with the grammar of `-> type`.
//
// so `()` is simply a function type that takes in no parameters and returns nothing.
//
// every function has its code static in the .exe and thus the storage of function type variables is simply a pointer to the function code.
//
// now, this kind of model could incur some performance penalties as the func would be called through a pointer stored in a variable.
// to this end, variables in PPL are "compile-time until proven otherwise".
// and for a compile-time function, PPL knows it can inline the function address at the call site.
//
// now, in the example above, there is a little bit of "variable capture" going on. this can be slightly complex, so there is a separate
// section in this document to explain this. FOR NOW, let's at least keep ourselves sane that this specific example is OK.
//
// the `exit` function has static storage duration and it captures `input_off` which also has static storage duration. OK.
// likewise for the `is_map_border`function, it has static storage duration and all that it captures too. OK.


SnakeBody :: struct {

    x : int = 0;
    y : int = 0;

// STATIC KEYWORD:
// ===============
//
// these modifier ideas always modify the type to the right.
    make_default := static () -> SnakeBody
    {

        return {-10; -10};
    }
// STRUCT MEMBER FUNCTIONS:
// ========================
//
// there is no vtable and no runtime polymorphism. the structs store the function pointers. these member functions
// compile under-the-hood to take as the first parameter a pointer to the type of the object. this is the `this` pointer.

};


// ROUTED VALUES and OBJECT INITIALIZATION:
// ========================================
//
buildBridge :: ( brickCount : int, height : float ) -> Bridge {
    return Bridge {};
}

// so this is what does not work.
//
// buildBridge(  brickCount = 10, height = 1.f  );
//
// the problem with this syntax is that there is a confusion between variable assignment
// of something that is already called e.g. `brickCount`.

// the solution:
//
// the `:` is used in other grammar construct to link an identifier to an object.
// so we could look at the idea here as linking the function parameter to the incoming object value.
buildBridge( brickCount : 10, height : 1.f );
//
// the `identifier : value` is actually a unique grammar syntax idea thus far. and therefore it could be a grammar object.
// maybe we call `identifier :` an "identifier link".
// so the runtime variable decl of before is actually `"identifier link" type = value`  OR  `"identifier link" type`
//
// so the `"identifier link" value` grammar construction is a "routed value", or maybe even a "named value".
//
vec := Vector {  x : 1.1f;  y : 1.2f;  }; // so this works too from the side of struct init.
//
// but now also consider,
vec = { x : 1.1f,  y : 1.2f };
//
// this is also grammatically valid. instead of the data pack containing many statements, there is just one statement of type Tuple.
// the assignment operator `=` will look at the data pack and "unpack" the stuff differently based on if it sees a Tuple or exp-statements.
//
// then we can look at array initialization:
//
arr2 := []int { 1, 2, 3, 5 }; // we do get to use commas here over `;`, which is nicer on the eyes.
//
// there is also a sort of special routing support for array init as well:
//
a2 := [10]u32 { [0] : 2, 3, [3] : 1 }; // gives an array of [ 2; 3; 0; 1; 0; 0; 0; 0; 0; 0 ].
//
// remember, everything is zero initialized unless explicitly marked as uninitialized.
//
A  := [10]u32 { [3] : 1, [0] : 2, 3 }; // gives compiler-error.
//
// the ^ above is not allowed. routed values to array slots must be in order when use with non-routed values.





// ------ ENUMS ------
Directions :: enum
{
    LEFT =    1;
    RIGHT; // 2
    UP;    // 3
    DOWN;  // 4
}
//
MyEnum :: enum_flag
{
    Flag1; // 1 << 0;
    Flag2; // 1 << 1; 
    Flag3; // 1 << 2;
}
// SYNTAX DETAIL:
// =======
//
// An enum is an an idea to declare a new type that can take a set of values.
//
// The rules are:
//   - All variables are type u64.
//   - We can also do this pattern where any subsequent member var is +1 the value of the previous
//     member var if not explicitly set.
//
// recall the syntax of `type data_pack`. So if we are going as `enum {}`, enum is a legit type.
// we could actually write the above as `Directions :: enum = enum {}`. that would be stupid, but it works.
// this means that the variable `Directions` has a type of `enum`.
//
// like the struct idea, such instances of compile-time enum variables create a new enum type for us.
// and the compiler adds into the `enum` object instance a member variable for each value option.
//
// thus, if we do an access like `Directions.LEFT`, this is a member read from the compile-time
// variable `Directions`. Since all this stuff is constant at runtime, the compile-time variable
// does not need to be stored and the enum values can be inlined at the site where they would have
// otherwise been a member access.


// IMPLICIT . SYNTAX:
// ==================
//
someBitFlag := MyEnum.Flag1 | .Flag2 | .Flag3;
// basically, we are allowed to omit the thing on the left side of a member access if possible to infer the thing to access.


// GENERICS:
// =========
//
snake := plib.Array<SnakeBody>;
//
// the ppl.types namespace implements a generic array type. it operates very simply, just like stb stretchy buffers.
// this generics idea is to compile different versions of a function based on a specific type.
//
genericAdd :: <T, Size : T> ( a : T, b : T ) -> T
{
    return a + b + Size;
}
//
// so the grammar for generics is like `identifier :: < list of generic types > ...`. our generic types can be either
// full-blown types or value type.


// FOR LOOPS:
// ==========
//
init_snake := () {
    // the `for` syntax takes a set-like to iterate over.
    // so the syntax is `for set-like statement`
    //
    // here we use a set construction syntax `..<` to build a set of integers (assumed u64).
    //
    for 0 ..< 5 // this loops through 0, 1, 2, 3, 4
    {
        body : SnakeBody = SnakeBody.make_default();
        snake.push(body);
    }

    snake[0] = SnakeBody {x : 5; y : 3};
}

move_snake := ()
{
// SWITCH STATEMENTS:
// ==================
//
    switch direction {
        case Directions.DOWN:  snake[0].y++;
        case Directions.LEFT:  snake[0].x--;
        case Directions.UP:    snake[0].y--;
        case Directions.RIGHT: snake[0].x++;
    }
//
// so the idea with the switch statement in this lang is default break behavior.
// fallthrough behavior must be explicit.

    for 1 .. snake.size
    {
// within the loop, there is an implicit value `it`. this is the current value in the set.
// there is also `it_index` which is the index of the current value in the set.
        snake[it] = snake[it - 1];
    }
}

snake_ate_food := () -> bool {
//
// both of Array<> as well as the static array [] type are set-like and therefore we can loop over them.
    for snake {
        if (it.x == foodX) && (it.y == foodY) then return false; // single statement `if` syntax.
    }
    return false;
}

this_func_is_not_called := ()
{
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

    for 0 .. 5 {
        for 0 .. 10 {
            // the second `it` within this loop shadows the first `it`.
        }
    }

    // so we introduce a new for loop syntax to allow for override the implicit `it`.
    for i in 0 .. 5
    {
        // `it_index` is still available.
        // so we also allow for override that too.
        for j, j_idx in 0 .. 6
        {
            // there is also a use-case where we can exit the outermost loop with this
            // sort of parameterized break statement.
            break i;
        }
    }

    // Can I have a runtime variable be equal to a set of integers?
    z : Span = 0 ..= 3; // so like, the set construction syntax gives back the Span type.
    // Span is just a struct of two integers :p
    z = Span { begin : 0; end : 3 }; // end is inclusive.

    b : []int = a[ z ];
    // if take a "slice" of an array, I just get back another array. this is natural since again all arrays are just
    // views. the base pointer and the count.

    for e, i in b; // this is a silly loop that does nothing :P

}


// RUNTIME TYPE INFO:
// ==================
//
typeExample :: () {
    varC := SnakeBody {};
    varA : Type = type_of(varC); 
    varB := bool;

    // the `Type` type is a first-class type.
    // the compilation meta-program keeps track of all the types. any variable with type `Type` can be set to any of the tracked types.

    // type_of() is a compiler built-in that returns the type of the expression.
    // also note that any compiler built-in along with any function as part of the standard library are snake case.

    if varA == varB {
        // we won't ever get here.
    }

    varA.name; // gives "s64" as "int" is an alias of that.
    // int must be an alias of this because we want literals to be strongly typed. if I type a large enough literal,
    // does it suddenly go from implicitly s32 to s64?? that would be bad.
    //
    // also since there is a difference between the max s64 and u64, if you type a thing that can fit in u64 but not s64,
    // then you actually must be explicit about the type (whether you declare in var or cast).


    info : TypeInfo = type_info(varA);
    // `type_info` is a compiler built-in that can get useful info about a Type.
    // the runtime variable instance of a Type does not store the info. it just stores a UID e.g.

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


// ENUM STRING STUFF:
// ==================
//
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
//
enum_stringify :: (dir : Directions) -> ^u8
{
    for type_info(Directions).members {

        memberValue := (^u8)@Directions + it.offset;
        //
        // recall that the `Directions` variable is a compile-time one and will also exist at runtime, so we can take its address.
        // the type info for an enum and the .offset is an offset within the compile-time variable.
        // so the above works as a way to iterate over the different value options of an enum.
        // and a similar strategy could be used for structs.
        // 
        if memberValue != dir then continue;

        return it.name; // the .name in any type info is baked into .exe, so OK to return.
    }
}


// the Any type:
// =============
//
theAny :: (any : Any) {
    any.type; // is of type `Type`.
    any.ptr;  // this is a void pointer to the memory for the thing.
}
//
// so any type in the language can be implicitly converted to the Any type if e.g. maybe a function parameter is of this type.


score : int = 0; // not needed due to zero initialization.
foodX : int;
foodY : int
direction : int = Directions.DOWN;
foodSymbolNum : int;


main := ( argc : int, argv : ^^char ) -> u32
{
    debug_print_directions();
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

    while true     // no `()` ever for if/for/switch... statements.
    {
        if hTerm.wait_until_input_ready()
        {

            switch hTerm.getchar() {
                case 'D': fall; // explicit fallthrough.
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

    tAttrFlagType = pal.TerminalFlag;
    currTermAttrs.flags &= ~( tAttrFlagType.INPUT_CANONICAL | tAttrFlagType.ENABLE_ECHO );
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

snake_eats_itself := () -> bool {

// TUPLES:
// =======
//
    head_x, head_y := snake[0].x, snake[0].y;
//
// so the idea here is that `,` is an operator. it takes the two expressions on either side and combines them
// to form a Tuple. a Tuple is a compile-time type. no runtime variable can be of type Tuple.
// when the assignment statement (or in a variable declaration context) and we see tuples on the sides, there is a sort
// of routing/slotting behavior.
//
// in fact, the input to every function call is a Tuple. so the function call operator knows what to do with that.

    for snake {
        if (it.x == head_x) && (it.y == head_y) then return true;
    }

    return false;
}

draw := ()
{
    hTerm := pal.get_or_create_terminal();
    hTerm.run("clear"); // synchronous.

    for i in 0 ..= MAP_HEIGHT
    {   
        for j in 0 ..= MAP_WIDTH
        {
            if is_map_border(j, i) then hTerm.print("#");
            else if is_snake_body(j, i) then hTerm.print("%c", SNAKE_BODY);
            else if is_food(j, i) {

                symbol := FOOD_SYMBOLS[foodSymbolNum];
                hTerm.print("%c", symbol);

            } else then hTerm.print(" ");
        }
        hTerm.print("\n");
    }

    hTerm.print("\n###### score: %d ######\n\n", score);
}

is_out_of_border := () -> bool
{
    return (snake[0].x == 0) || (snake[0].y == 0) || (snake[0].x == MAP_WIDTH) || (snake[0].y == MAP_HEIGHT);
}

is_snake_body := (x : int, y : int) -> bool
{
    for snake {
        if (it.y == y) && (it.x == x) then return true;
    }
    return false;
}

is_food := (x : int, y : int) -> bool
{
    return (foodX == x) && (foodY == y);
}

input_off := ()
{
    hTerm := pal.get_or_create_terminal();
    hTerm.reset_attributes();
}

