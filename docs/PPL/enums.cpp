enum Pokemon { PIKACHU = 0, CHARMANDER, SQUIRTLE, BULBASAUR };
// enums define a type.
// just like a struct.
// the value of the type is restricted to the value set defined in the enum.
// but the type of these restricted values is always uint32_t.
// so it's basically a quantized uint32_t, in a way.

// TODO: do we want to allow for other data types in enums?

// TODO: I recall that Java has some interesting things going on with enums. we
// should check this out.

int main() {
    // enum identifiers are not thrust into the global namespace.
    Pokemon p1 = Pokemon::PIKACHU;
    Pokemon p2 = Pokemon::CHARMANDER;
    Pokemon p3 = Pokemon::SQUIRTLE;
    Pokemon p4 = Pokemon::BULBASAUR;

    // common enum ideas that are difficult to implement in C:
    // 1. iterating over the enum values.
    // 2. getting the name of an enum value.

    // can we get these to work?

    // ------------- ENUM STRINGIFICATION -------------

    // introducing the "stringify" operator: '##'
    // this is a unary operator that works on any built-in type,
    // but the expression value must be constexpr.
    // we chose '##' because '#' is for preprocessor macros.
    // '##' happens DURING compilation.
    constexpr ^char p1Name = ##Pokemon::PIKACHU;

    // more example of "stringify" operator:
    ^char intString = ##123;  // in general, we could do this with any literal.
    ^char intString2 = ##(123 + 100); // produces "223".

    // the details of conversion are of course, "what you would expect".
    // we'll need to have some amazing docs for this.
    // the way I am thinking we could do this is to take the type of the operand
    // to '##', then do a "default format string" for that type. like if you
    // wrote %f and gave a float. if '##' sees a float it does a %f. so we
    // piggypack on the format string stuff to get a clear definition for our
    // operator.

    // this actually means we might want to allow for format string stuff to happen.
    ^char reminderString = "this" "still" "works"; // works with constexpr string literals.
    ^char intString3 = "0x" ##%x(123 + 100); // produces "0xdf"
    // '##%x' is a composite operator.

    // TODO: does the above syntax make parsing difficult?
    // could there be ambiguities with other syntaxes??
    // I want someone to try to break this lol.

    // of course, with string formatting and the sort, lots of people like to
    // be able to just see the format string in its raw form.
    // so we can do this:
    ^char usingRawFormatStrings = cprintf("0x%x", (123 + 100)); // produces "0xdf"
    // cprintf is printf but with constexpr args.
    // its a compiler built-in function.

    // and finally,
    // what if we borrow template literals from Javascript???
    ^char templateLiteral = `Hello, ${p1Name}!`;
    // of course we can!!! ${} is actually going to insert the value, which must be constexpr,
    // and do an implicit `##` on it.


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

    uint32_t numPokemon = countof(Pokemon);
    // count is one of these "function like" language built-ins. Like sizeof().

    // here, we demonstrate the "countof" the enum. but we can get the
    // "countof" a lot of other types as well. Arrays are a good example.
    // whether they are static or dynamic.
}