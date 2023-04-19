
// all symbols are automatically exported (unless static) from a compilation unit.
// but no namespaced symbols are automatically imported unless explicitly declared as such.

pc : #import "ppl.console";
// ^ this marks the ppl.console namespace as explicitly imported.
// ^ this import creates a new namespace `pc` that aliases `ppl.console`.

#import "ppl.console"; // this is a thrust into the global namespace.
// it's kind of like a using statement.

pc.print("Hello, world");
print("Hello, World!");

// legacy #include also works.
// this is a direct include as early as possible in the compiler pipeline.
// works based on the filesystem.
#include "math.h"

main : {
    // imports work anywhere. they thrust this namespace into whatever
    // namespace the statement is within.
    #import "ppl.array";

    thing : array<int> = {};
}

thing2 : array<int> = {}; // doesn't work.

// TODO:
// header files are kind of annoying.
// I see this as duplicate code.
// the purpose would be for a library that
// wanted to have the header file serve as documentation.
