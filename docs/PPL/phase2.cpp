// Copyright (c) 2023, Noah Cabral. All rights reserved.
//
// This file is part of the Portable Programming Language compiler.
//
// See file LICENSE.md for full license details.


// note the different syntax for compiler-variables.
// in the type inference case, we can omit the `=`.
WORLD_SCALE :: 1;
CHUNK_SIZE  :: 16;
WORLD_SIZE  :: 16;

DOT_WORLD_MAGIC_NUMBER :: 0x444C524F57544F44;  // DOTWORLD

// enum_flags is very clean!
WorldCubeEdge :: enum_flags
{
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    TOP_FRONT,
    TOP_BACK,
    BOTTOM_FRONT,
    BOTTOM_BACK,
    FRONT_LEFT,
    FRONT_RIGHT,
    BACK_LEFT,
    BACK_RIGHT,
}

// in the type inference case the equal sign is required again.
WORLD_CUBE_EDGE_UNDEFINED :: u32 = UINT32_MAX;

// the beautiful implicit . syntax :D
WORLD_CUBE_EDGE_FACE_FRONT  :: WorldCubeEdge.TOP_FRONT | .BOTTOM_FRONT | .FRONT_LEFT | .FRONT_RIGHT;

WORLD_CUBE_EDGE_FACE_BACK   :: WorldCubeEdge.TOP_BACK | .BOTTOM_BACK | .BACK_LEFT | .BACK_RIGHT;

WORLD_CUBE_EDGE_FACE_LEFT   :: WorldCubeEdge.TOP_LEFT | .BOTTOM_LEFT | .FRONT_LEFT | .BACK_LEFT;

WORLD_CUBE_EDGE_FACE_RIGHT  :: WorldCubeEdge.TOP_RIGHT | .BOTTOM_RIGHT | .FRONT_RIGHT | .BACK_RIGHT;

WORLD_CUBE_EDGE_FACE_TOP    :: WorldCubeEdge.TOP_FRONT | .TOP_BACK | .TOP_LEFT | .TOP_RIGHT;

WORLD_CUBE_EDGE_FACE_BOTTOM :: WorldCubeEdge.BOTTOM_FRONT | .BOTTOM_BACK | .BOTTOM_LEFT | .BOTTOM_RIGHT;


// DOT WORLD FILE FORMAT v0.1.0
// dot_world_header_t, immediately followed by numChunks dot_world_chunk_info_t, then the chunk_t's.

// TODO: I'm not sure how I feel about this syntax.
[[pack(1)]] { // everything inside this block is packed to 1 byte.

    dot_world_header_t :: struct {
        magicNumber   : [8]u8   = {0x44; 0x4F; 0x54; 0x57; 0x4F; 0x52; 0x4C; 0x44};  // DOTWORLD
        versionNumber : [9]u8   = {0x30; 0x2E; 0x31; 0x2E; 0x30; 0x0};               // 0.1.0 (null-terminated)
        numChunks     : u32     = 0;
    }

    dot_world_chunk_info_t :: struct  {
        chunkX : u32 = 0;
        chunkY : u32 = 0;
    }
}

AtomKind :: enum
{
    UNDEFINED = 0;
    GRASS_BLOCK = 1;
    DIRT_BLOCK = 3;
    SAND_BLOCK = 5;
    BLUE_FLOWER_FOLIAGE_BLOCK = 13;
    BULB_FLOWER_FOLIAGE_BLOCK = 14;
    PURPLE_THNEED_FOLIAGE_BLOCK = 28;
    ORANGE_THNEED_FOLIAGE_BLOCK = 29;
    YELLOW_THNEED_FOLIAGE_BLOCK = 30;
    WHITE_THNEED_FOLIAGE_BLOCK = 31;

    COBBLESTONE_BLOCK = 16;
    STONE_BLOCK = 17;

    GOLD_ORE_BLOCK = 18;
    IRON_ORE_BLOCK = 19;
    COAL_ORE_BLOCK = 20;
    DIAMOND_ORE_BLOCK = 21;

    BRICK_BLOCK = 22;
    SNOWY_STONE_BLOCK = 23;

    WOODEN_PLANK_BLOCK = 32;
    THNEED_LOG1_BLOCK = 33;
    THNEED_LOG2_BLOCK = 34;

    PURPLE_LAMP_BLOCK = 48;
    BLUE_LAMP_BLOCK = 49;
    GREEN_LAMP_BLOCK = 50;
    RED_LAMP_BLOCK = 51;

    GLASS_BLOCK = 64;
    ICE_BLOCK = 65;
    SNOW_BLOCK = 80;

    WHITE_THNEED_BLOCK = 96;
    YELLOW_THNEED_BLOCK = 97;
    RED_THNEED_BLOCK = 98;
    PURPLE_THNEED_BLOCK = 99;

    MAX_COUNT = 256;
}

WorldCubeFace :: enum_flags
{
    FRONT,
    BACK,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
}

atom_kind_render_info_t :: struct {
    // NOTE: both isBlock and isFoliage can be true.
    isBlock            := false;  // Render as a block.
    isFoliage          := false;  // Render as a foliage.
    faceOffsetTable : [6]int = {};     // if block, what faces to render with relative to based render idx.
                                       // ORDER: front, back, left, right, top, bottom.
}

atom_kind_info_t :: struct {

    // if name=nullptr this is an atom that does not exist. sparse entry in table.
    Name  : ^u8 = nullptr;
    RenderInfo : atom_kind_render_info_t = {};

    // TODO: maybe types can just be printed via a lib?
    // there shouldn't be any variability in the way that a type prints.
    print :: (
        printerFn : (fmt : ^u8, args : Any...)
    ) {
        // even though this is PPL let's suppose we have std.string. it is convenient.
        computeArrString :: (arr : []int) -> std.string {
            std.string s = "{";
            for arr {
                s += std.to_string(it);
                if (it_index != arr.size - 1) s += ", ";
            }
            s += "}";
            return s;
        };
        printerFn(
            "{\n\tname=%s,\n\trenderInfo={\n\t\tisBlock=%d\n\t\tisFoliage=%d\n\t\tfaceOffsetTable=%s\n\t}\n}\n",
            Name,
            RenderInfo.isBlock,
            RenderInfo.isFoliage,
            computeArrString(RenderInfo.faceOffsetTable).c_str()
        );
    }

}

// purposefully not `::` and instead `:`.
// using just `:` actually makes this an anonymous struct runtime variable.
atom_kind_info_table_t := struct {
    infos : [AtomKind.MAX_COUNT]atom_kind_info_t = {};
    get :: (Idx : AtomKind) -> ^atom_kind_info_t { return @infos[Idx]; }
    get :: (Idx : u64 ) -> ^atom_kind_info_t { return @infos[Idx]; }
}

// this is the same mechanism. we use the type to the left of the data pack
// to type that data pack, then use the type inference mechanism for the variable
// decl.
AtomIterKind := (kind : AtomKind) -> AtomKind
{
    for kind + 1 .. AtomKind.MAX_COUNT {
        if (atom_kind_info_table_t.get(i).Name) break;
    }
    // implicit casting like this is OK so long as there is no data loss.
    return i;
}

// the below is the more explicit version of what the hell is happening.
generateAtomKindInfoTable :: () -> void = {
    if #compile_time
    {
        isAtomKindFoliage := (str : ^u8) -> bool
        {
            return doesStringContainString(str, "FOLIAGE");
        }

        // remember, variables are constant until proven otherwise.
        isAtomKindBlock := (str : ^u8) -> bool
        {
            return doesStringContainString(str, "BLOCK");
        }

        // even in a compile-time context, AtomKind is considered a Type.
        // in a way, this is a little bit of polymorphism.
        // but this sort of polymorphism only works in a compile-time context.
        typeInfo : TypeInfo = type_info(AtomKind);
        for typeInfo.members {
            // we are only able to take the address of AtomKind from within a compile-time
            // context, as AtomKind is a compile-time variable.
            kind := cast<^u8>(@AtomKind) + it.offset;
            ^atom_kind_info_table_t.get(^kind) = {
                it.name; // type info is static in .exe
                {
                    isBlock : isAtomKindBlock(it.name);
                    isFoliage : isAtomKindFoliage(it.name)
                }
            }
        }

        // assignment with any data packs `{}` just works :).
        atom_kind_info_table_t.get(ATOM_KIND_GRASS_BLOCK).RenderInfo.faceOffsetTable = {1; 1; 1; 1; 0; 2}
        atom_kind_info_table_t.get(ATOM_KIND_DIRT_BLOCK).RenderInfo.faceOffsetTable = {1; 1; 1; 1; 0; 0}
        atom_kind_info_table_t.get(ATOM_KIND_SNOWY_STONE_BLOCK).RenderInfo.faceOffsetTable = {
            0; 0; 0; 0;
            int(ATOM_KIND_SNOW_BLOCK - ATOM_KIND_SNOWY_STONE_BLOCK);
            int(ATOM_KIND_STONE_BLOCK - ATOM_KIND_SNOWY_STONE_BLOCK);
        }
        atom_kind_info_table_t.get(ATOM_KIND_THNEED_LOG1_BLOCK).RenderInfo.faceOffsetTable = { 0; 0; 0; 0; 2; 2; }
        atom_kind_info_table_t.get(ATOM_KIND_THNEED_LOG2_BLOCK).RenderInfo.faceOffsetTable = { 0; 0; 0; 0; 1; 1; }
    }
}

// compile-time execution with effects (side effects to global variables).
#run_with_effects generateAtomKindInfoTable();


debugPrintAtomKindInfos :=
{
    AELogger("atom_kind_info_table_t:\n");
    
    // while loops can have that old for loop syntax.
    while i := AtomKind {}; i < AtomKind.MAX_COUNT; i = AtomIterKind(i);
    {
        // also note this AtomKind {} idea. this is a data pack with type of AtomKind.
        // kind of like if you write struct {}, that is a data pack that is an anonymous struct.
        // or namespace {}, which is a data pack that is an anonymous namespace.

        info := atom_kind_info_table_t.get(i);
        printerFn : (fmt : ^u8, args : Any...) {
            ae.platform.fprintf_proxy(ae.platform.AE_STDOUT, args...); 
        };
        info.print(printerFn);
    }
}

// -------------

// so there are like tons of ideas that we can pull from other languages that I
// think we would be silly to ignore.

// I want to see if we can pull all the language ideas into a really, really well
// designed thing. that sounds like quite a compelling idea to me. so lets give it a go.

// and while we are at it, yknow we can also tackle the whole computer graphics thing
// really well. I think that this is a nice problem space to take a look at.


// we demonstrate below some QOL features:

// reason for multiple return:
//   - you need to return a thing but also an error. it is only sometimes that the error
//     can be represented as a special value of that thing. other times the error and thing
//     are separate types, and they should be.

// now, you can literally just have pointers. right, and just have the calling function write to
// those. but there is some nice syntactic sugar with multiple return. is that too high level
// however?

// this works with a jump.
// then you just need to coordinate the passing and return of data.
// so, you can pass stuff by register.
// you can have a call stack and pass stuff that way.
// you can return one value and do so by register.
// to "return" multiple values, it is again you either play around with the stack or do register.

buildChickens ::
// so functions can have this special sort of type where they return multiple things.
(eggCount : int, isReallyOld : bool) -> (interesting: int, syntax : float)
{
    // this is what it would look like within the function.
    interesting = 2;
    syntax = 1.f;
    return;
}

// how it looks on the other side.
s, s2 := buildChickens( eggCount=3, isReallyOld=true );

// if we only wanted just one.
s3 := buildChickens().syntax + 2.f;

// under the hood this is actually a pretty simple idea.
// the function params are passed on the stack. for the return params, you can just
// push some more things on the stack. but these are the pointers to where you are returning into.
// if you are not going to return into anything, then the compiler can optimize out stuff.
// that pointer could either be to a real variable or if needed, a temporary.

// this is feasible. it's not like an out to lunch idea. it's gonna be like just as fast
// as if you passed a bunch of pointers to the function anyways. like, that would be what you
// would have written.


// how the COMMA SHIT works.
// =========================
//
// the `=` can take expressions with `,` on the LHS and RHS, then it just routes things.
// a function with multiple return values will expand as if it had the comma.

// what if I try to nest those ideas?
(a,b) , (c,d) = buildChickens(), buildChickens();
// so any nesting should just unfold to like a flat sequence,
// then it is a simple routing problem. both sides should do the unfold.

// on the language purity side of things and for the comma idea ...
// suppose that the comma was an operator.
// this returns a particular expression type.
// we could just define the `=` operator nicely on that type.
// then we could have this idea where this type that is the result of comma op is strictly a
//   compilation meta-program type. you cannot have a runtime variable of this type.
// the type is Tuple.

type_of( a, b ) == Tuple; // true

#if 0 {
    thing := a, b; // compiler error.
}

// TODO:
// When you look at a function type, are those Tuple literals?

// TODO:
// how can we make sense of the named parameter stuff ??

// I think what is going on here is that you have the string in the AST, right, then you do the lookup
// to see if you can find the `Id`. but, suppose that it cannot be found. in such a case, maybe things
// do not immediately fail. maybe we wait to see if something might happen to "recover" us. and it could
// be the very linkage of that Tuple as the input into a function for those unresolved string to finally
// get resolved. so like, once we start defining the language as a thing, and filling in gaps with the
// compilation metaprogram, things seem to be nice :D

// TODO: When you look at the member access syntax to get just one elem of the multi-return, is that OK?
// my guess for this is yes. because you are member access into a tuple. a Tuple is just a struct
// kind of idea. a package of data that is const. no big deal there. and things also work because the 
// `=` operator knows what to do with the Tuple if you leave it as is and do no such member access.


// the whole idea of C is that the compilation metaprogram exists. without it, we cannot do variable
// lookup which is like the thing where all the power is. the idea of a context and a history of
// language statements that give meaning to the statement that is one the particular line.
// most langs have a context.

// TODO: going to need raw string literals (the escapes like \n do not apply)?
str := r"\nmaybe this"; // ?
// but couldn't I just
str2 := "\\nmaybe this"; // ?

// TODO: and we do need to think about an actual string type that is not null-terminated one.
// so we could have a `String` type. this is just an alias for []char. 
// which is an array of characters (which is a view since all array types are views).


// TODO: Odin has a nice range syntax that is a little nicer than in JAI. consider e.g.
for i in 0 ..< 10 {
    // so i goes [0,9]
}
// I think this kind of explicitness is good as opposed to 0 .. 10, what is that? inclusive or exclusive? 
for i in 0 ..<= 10 {
    // i goes [0,10]
}
// so we could get rid of the `..` operator altogether and go with the explicit ones above.

// TODO: just like dynamic arrays a standrt



// TODO:
// STRING TYPE:
// ============


// TODO:
// MEMORY ALLOCATION:
// ==================
//
// dynamic memory allocation is an operating system specific function.
// therefore, there is no `new` keyword.
// the best that we can do is have the `malloc` function as part of the standard lib.
memoryStuff :: namespace {
    F := malloc(int, 10); // malloc first parameter is `Type`.
    defer free(F);

    // TODO: JAI has this idea of a context where a context has an assigned memory allocator.
    // maybe that is an interesting idea.

    // unless we do the memory context thing, dynamic arrays _cannot_ be a core part of the language.
    // they must be a part of the standard lib of the lang.
}


// TODO:
// OPERATOR OVERLOADING:
// =====================
//
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

// TODO:
// VARIABLE CAPTURES:
// ==================
// 
// so it matters whether the function is a compile-time one or a runtime-one.
//
// if it is a runtime function and we wanted to support capture by value, I would imagine that there would be a sort of code-injection
// idea. any maybe there need to be multiple copies of the function code to support N different objects of the runtime func?
//
// but at any rate,
//
// Also note that "variable capture" is a default thing. In C++ parlance, our "variable capture" is capture by reference.
// I can imagine that on the implementation side that "capture by reference" is just capturing the `Id`, after variable lookup.
// and consequentially, this default variable capture has a limitation to prevent dangerous situations.
// so the rule is that the storage duration of the thing being captured is less than or equal to that of the function.


// TODO:
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
//
// SYNTAX DETAIL:
// =======
//
// These are semantic macros. They are shamelessly ripped-off from JAI.
// A macro is this saved chunk of AST that can be inserted/inlined later.
// This allows creating functions / constants / things that are always inlined.
//
// the #save_code runs at compile time to grab the thing and save the AST of that.
// then when later evaluating MAP_WIDTH as a value, we can insert the AST there to get the value.


// TODOs:
//
// - Would we want 128 size support?
// - Explore array decay to pointer with the #as idea with structs?
// - Add ideas for how to pack a struct.
// - Should enums be allowed to have member functions and is this valuable?
// - AoS versus SoA refactor?
// - Right now we have C-style casting. Revisit that. Also look at truncation.
// - Maybe it would be a good idea to look at a print context.
// - The terminal abstraction in the PAL is likely not fully platform agnostic to how terminals work on all platforms.

var0  := "haha 😂";     // TODO.
var1  := '👌';          // TODO.

// TODO:
// when it comes to for loops, need to use more ideas from the JAI:
//   - remove and insert into set as iterate.
//   - iterate over the set backwards.
//   - the idea that for loops are just a macro thing so that you can extend to allow more types to be set-like.
