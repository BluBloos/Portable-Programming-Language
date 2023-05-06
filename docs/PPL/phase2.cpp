#if 0
{
a24   := 0b10;        // this is an int.
a25   := 1_000_000;   // this is an int.
a26   := 1.0e9;       // this is a  float.
var3  := 0xFA;        // this is an int.
}

#if 0 {
// this is the legacy include idea. a simple file-system based copy-pasta.
#include "someFile.ppl";

// compat for foreign things.
legacyMath :: #translate_c99 #include "math.h";
}

// DUFFS DEVICE:
// =============
//
#if 0 {
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
//
// SYNTAX:
// =======
//
// The thing to note about the example above is that the case labels are not required to
// be directly descendant to the switch statement in the AST. they can be anywhere pretty much
// and just denote a location to jump to.


// TODO:
//
// there is also a sort of special routing support for array init as well:
//
a2 := [10]u32 { [0] = 2, 3, [3] = 1 }; // gives an array of [ 2; 3; 0; 1; 0; 0; 0; 0; 0; 0 ].
//
// remember, everything is zero initialized unless explicitly marked as uninitialized.
//
A  := [10]u32 { [3] = 1, [0] = 2, 3 }; // gives compiler-error.
//
// the ^ above is not allowed. routed values to array slots must be in order when use with non-routed values.