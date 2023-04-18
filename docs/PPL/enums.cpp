Pokemon : enum { PIKACHU = 0, CHARMANDER, SQUIRTLE, BULBASAUR };
// enums define a type.
// just like a struct.
// the value of the type is restricted to the value set defined in the enum.
// but the type of these restricted values is always uint32_t.
// so it's basically a quantized uint32_t, in a way.

// TODO: do we want to allow for other data types in enums? e.g. u64.

// TODO: I recall that Java has some interesting things going on with enums. we
// should check this out.

main : {

    p1 : Pokemon = Pokemon.PIKACHU;
    p2 := Pokemon.CHARMANDER;
    p3 := Pokemon.SQUIRTLE;
    p4 := Pokemon.BULBASAUR;

    // common enum ideas that are difficult to implement in C:
    // 1. iterating over the enum values.
    // 2. getting the name of an enum value.

    // can we get these to work?

    // ------------- ENUM STRINGIFICATION -------------

    // introducing the "stringify" operator: '##'
    // this is a unary operator that works on any built-in type,
    // but the expression value must be constexpr.
    // we chose '##' because '#' is for preprocessor macros.
    // '##' works on things that are known at compile-time.
    // and produces something that is known at compile-time.
    p1Name! : ^char = ##Pokemon.PIKACHU;

    // more example of "stringify" operator:
    intString : ^char   = ##123;         // in general, we could do this with any literal.
    intString2 : ^char  = ##(123 + 100); // produces "223".

    // the details of conversion are of course, "what you would expect".
    // we'll need to have some amazing docs for this.
    // the way I am thinking we could do this is to take the type of the operand
    // to '##', then do a "default format string" for that type. like if you
    // wrote %f and gave a float. if '##' sees a float it does a %f. so we
    // piggypack on the format string stuff to get a clear definition for our
    // operator.

    // this actually means we might want to allow for format string stuff to happen.
    reminderString : ^char = "this" "still" "works"; // works with compile-time string literals.
    intString3 : ^char = "0x" stringify("%x", 123 + 100); // produces "0xdf"

    // so in fact, `##` is just a shorthand for `stringify`, which is this compiler intrinsic.

    // ------------- ITERATING OVER ENUM VALUES -------------

    // so for this, we essentially want to be careful of not adding a hidden
    // cost. hiding costs is the key to taking this language, which is meant to
    // be low-level, and turn it into a high-level language.

    // if our enum values are 0,1,2,3,4,5,6,...
    // this is OK to iterate over. no hidden costs here.

    // however, if the enum set is disjoint, then we have a problem.
    // does the compiler "figure out the math" to generate the correct values?
    // or does it use a lookup table?
    // either method incurs a hidden cost.
    // and we should leave such iteration to the user.

    // however, there is still work to do on this front.
    // namely, I simply want to know _how many_ enums there are.

    numPokemon : uint32_t = countof(Pokemon);
    // count is one of these "function like" language built-ins. Like sizeof().

    // here, we demonstrate the "countof" the enum. but we can get the
    // "countof" a lot of other types as well. Arrays are a good example.
}