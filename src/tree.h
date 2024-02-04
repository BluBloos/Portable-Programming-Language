#ifndef TREE_H // TODO: make this file a .hpp
#define TREE_H

#include <ppl_types.hpp>

// TODO(Noah): I feel like there is something we can do here.
// All of this is pass thru for some tokens right to tree metadata.
// like, TOKEN_QUOTE -> AST_STRING_LITERAL
// there is basically nothing special that happens here.

enum tree_type {
    TREE_ROOT = 0,
    TREE_REGEX_STR,
    TREE_REGEX_ANY,
    TREE_REGEX_GROUP,
    TREE_REGEX_CHAR,
    TREE_REGEX_KEYWORD,

    TREE_REGEX_GENERIC_KEYWORD,
    TREE_REGEX_LITERAL,
    TREE_REGEX_SYMBOL,
    TREE_REGEX_OP,

    AST_GNODE,

    //AST_CHARACTER_LITERAL,

    AST_INT_LITERAL,
    AST_DECIMAL_LITERAL,

    AST_STRING_LITERAL,
    AST_NULL_LITERAL,

    AST_SYMBOL,
    AST_OP,
    AST_KEYWORD
};

struct tree_metadata {

    char regex_mod;

    // value.
    union { // kind of like the data storage for tree.
        UNICODE_CPOINT c;
        const char *str;
        uint64 num;
        double dnum; // stores all decimals (float/double).
    };

    // value-kind.
    ppl_type valueKind;

};

/*
Okay, this is the thing that I did not want to do. but I have been forced by to
do this because, well....I do not know what else to do...

So here is what must be done:
    - Make a struct for a tree_node (this guy is the full deal man...the full tree).
    - This struct contains inside it a list of children (each of this are dynamic lists).
    - And every element of children is another tree_node.
    - Okay and that's it!
*/

struct tree_node {
  public:
    /* psuedo-linked list style */
    struct tree_node *children;
    unsigned int childrenCount;
    unsigned int childrenContainerCount;
    /* linked list style */

    enum tree_type type;
    struct tree_metadata metadata;
};

// TODO: I do not know if these CreateTree calls are the best idea.

struct tree_node CreateTree(enum tree_type type) {

    struct tree_node tn =
        {}; // initialize everything to zero for sanity purposes.
    tn.type = type;
    tn.children = NULL;
    tn.childrenCount = 0;
    tn.childrenContainerCount = 0;
    tn.metadata.regex_mod = 0; // null character (null-terminator).
    tn.metadata.valueKind = PPL_TYPE_UNKNOWN;
    return tn;
}

struct tree_node CreateTree(enum tree_type type, UNICODE_CPOINT c) {
    struct tree_node tn = CreateTree(type);
    tn.metadata.c = c;
    tn.metadata.valueKind = PPL_TYPE_U32;
    return tn;
}

struct tree_node CreateTree(enum tree_type type, char c) {
    struct tree_node tn = CreateTree(type);
    tn.metadata.c = c;
    tn.metadata.valueKind = PPL_TYPE_S8;
    return tn;
}

struct tree_node CreateTree(enum tree_type type, const char *str) {
    struct tree_node tn = CreateTree(type);
    tn.metadata.str = MEMORY_ARENA.StringAlloc((char *)str);
    // TODO: currently we are using PPL_TYPE_UNKNOWN and I think this is OK?
    return tn;
}

struct tree_node CreateTree(enum tree_type type, int64_t num) {
    struct tree_node tn = CreateTree(type);
    tn.metadata.num = num;
    tn.metadata.valueKind = PPL_TYPE_INT;
    return tn;
}

struct tree_node CreateTree(enum tree_type type, uint64 num) {
    struct tree_node tn = CreateTree(type);
    tn.metadata.num = num;
    tn.metadata.valueKind = PPL_TYPE_U64;
    return tn;
}

struct tree_node CreateTree(enum tree_type type, double dnum) {
    struct tree_node tn = CreateTree(type);
    tn.metadata.dnum = dnum;
    tn.metadata.valueKind = PPL_TYPE_DOUBLE;
    return tn;
}

struct tree_node CreateTree(enum tree_type type, float fnum) {
    struct tree_node tn = CreateTree(type);
    tn.metadata.dnum = fnum;
    tn.metadata.valueKind = PPL_TYPE_FLOAT;
    return tn;
}

struct tree_node CreateTree(enum tree_type type, bool b) {
    struct tree_node tn = CreateTree(type);
    tn.metadata.num = b;
    tn.metadata.valueKind = PPL_TYPE_BOOL;
    return tn;
}

// Needs to be a full-blown tree that gets adopted. Not just a leaf node.
void TreeAdoptTree(struct tree_node &tn, struct tree_node child) {
    // TODO(Noah): Maybe abstract stuff and make a dynamic list type :)
    // because this same type of pattern is also done in tokenContainer.
    if (!(tn.childrenCount < tn.childrenContainerCount)) {
        if (tn.childrenCount > 0) {
            tn.childrenContainerCount += 100;
            struct tree_node *_trees = (struct tree_node *)realloc(
                tn.children,
                tn.childrenContainerCount * sizeof(struct tree_node));
            Assert(_trees != NULL);
            tn.children = _trees;
        } else {
            // doesn't make sense to realloc memory that has not been alloc yet.
            // so here we do a malloc to init the array.
            tn.childrenContainerCount = 100;
            struct tree_node *_trees = (struct tree_node *)malloc(
                tn.childrenContainerCount * sizeof(struct tree_node));
            Assert(_trees != NULL);
            tn.children = _trees;
        }
    }
    memcpy(&tn.children[tn.childrenCount++], &child, sizeof(struct tree_node));
}

// TODO(Noah): Test this function. We wrote it and observed our program
// working. But it only seems to be working. Maybe there is some wacky things going
// on.
void DeallocTree(struct tree_node &tn) {
    if (tn.children == NULL) {
        return;
    } else {
        for (unsigned int i = 0; i < tn.childrenCount; i++) {
            DeallocTree(tn.children[i]);
        }
        free(tn.children);
    }
}

static void PrintTree_DefaultPrinter(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    LOGGER.vMin(fmt, args);
    va_end(args);
}

void PrintTree(const struct tree_node &tn, unsigned int indentation, 
    void (*printFn)(const char *fmt, ...) = nullptr )
{
    // TODO: printFn needs va_list kind of thing.
    auto PrintFn = (printFn) ? printFn : PrintTree_DefaultPrinter;

    // NOTE(Noah): Max indentation is 80 ' ' . Any deeper AST get like, "truncated", or whatever.
    char sillyBuff[81]; // 1 extra than 80 for the null-terminator.
    unsigned int i;
    for (i = 0; i < indentation && i < 80; i++) {
        sillyBuff[i] = ' ';
    }
    sillyBuff[i] = 0; // null terminator.

    switch (tn.type) {

        case TREE_ROOT:
            // in this case we do nothing. in the code below this switch statement,
            // we will recurse and print all the children of a root node.
            break;

        case TREE_REGEX_STR:
            Assert(tn.metadata.str != NULL);
            PrintFn("%sSTR:%s", sillyBuff, tn.metadata.str);
            break;
        case TREE_REGEX_ANY: PrintFn("%sAny", sillyBuff); break;
        case TREE_REGEX_GROUP: PrintFn("%sGroup", sillyBuff); break;
        case TREE_REGEX_CHAR:
            PrintFn("%sCHAR:%c", sillyBuff, tn.metadata.c);
            break;
        case TREE_REGEX_KEYWORD:
            Assert(tn.metadata.str != NULL);
            PrintFn("%sKEYWORD:%s", sillyBuff, tn.metadata.str);
            break;

        // TODO: there is likely a nice metaprogramming idea here.
        // all the things to print are the enum names but trim a little of the front. 
        case AST_GNODE:
            Assert(tn.metadata.str != NULL);
            PrintFn("%sGNODE:%s", sillyBuff, tn.metadata.str);
            break;

#if 0
        case AST_CHARACTER_LITERAL:
            PrintFn("%sCHARACTER_LITERAL:%c", sillyBuff, tn.metadata.c);
            break;
#endif

        case AST_INT_LITERAL:
            PrintFn("%sINT_LITERAL:%" PRIu64 ", valueKind:%s",
                sillyBuff,
                tn.metadata.num,
                PplTypeToString(tn.metadata.valueKind));
            break;

        case AST_DECIMAL_LITERAL:
            PrintFn("%sDECIMAL_LITERAL:%f, valueKind:%s",
                sillyBuff,
                tn.metadata.dnum,
                PplTypeToString(tn.metadata.valueKind));
            break;

        case AST_STRING_LITERAL:
            Assert(tn.metadata.str != NULL);
            PrintFn("%sSTRING_LITERAL:%s", sillyBuff, tn.metadata.str);
            break;
        case AST_NULL_LITERAL:
            PrintFn("%sNULL_LITERAL", sillyBuff);
            break;
        
        case AST_SYMBOL:
            Assert(tn.metadata.str != NULL);
            PrintFn("%sSYMBOL:%s", sillyBuff, tn.metadata.str);
            break;
        case AST_OP:
            Assert(tn.metadata.str != NULL);
            PrintFn("%sOP:%s", sillyBuff, tn.metadata.str);
            break;
        case AST_KEYWORD:
            Assert(tn.metadata.str != NULL);
            PrintFn("%sKEYWORD:%s", sillyBuff, tn.metadata.str);
            break;
        
    }

    if (tn.metadata.regex_mod > 0) {
        PrintFn("%c\n", tn.metadata.regex_mod);
    } else {
        PrintFn("\n");
    }

    if (tn.children != NULL) {
        for (unsigned int i = 0; i < tn.childrenCount; i++) {
            PrintTree(tn.children[i], indentation + 2, PrintFn);
        }
    }
}

#endif
