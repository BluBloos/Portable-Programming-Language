#ifndef TREE_H // TODO: make this file a .hpp
#define TREE_H

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
    
    // NOTE: why is there duplication of the token op enums and the ast op enums?
    // the reason is that they mean different things! so, the ast nodes exist to indicate structure.
    // so if I have an ast node for function call, there is one of those but two tokens that go as  "(" and ")".
    AST_OP_MEMBER_SELECTION, // "."
    AST_OP_FUNCTION_CALL, // "()"
    AST_OP_ARRAY_SUBSCRIPT, // "[]"
    AST_OP_MULTIPLICATION, // "*"
    AST_OP_DIVISION, // "/"
    AST_OP_MODULUS, // "%"
    AST_OP_ADDITION, // "+"
    AST_OP_SUBTRACTION, // "-"
    AST_OP_BITWISE_LEFT_SHIFT, // "<<"
    AST_OP_BITWISE_RIGHT_SHIFT, // ">>"
    AST_OP_LESS_THAN, // "<"
    AST_OP_LESS_THAN_OR_EQUAL_TO, // "<="
    AST_OP_GREATER_THAN, // ">"
    AST_OP_GREATER_THAN_OR_EQUAL_TO, // ">="
    AST_OP_EQUAL_TO, // "=="
    AST_OP_NOT_EQUAL_TO, // "!="
    AST_OP_BITWISE_AND, // "&"
    AST_OP_BITWISE_XOR, // "^"
    AST_OP_BITWISE_OR, // "|"
    AST_OP_LOGICAL_AND, // "&&"
    AST_OP_LOGICAL_OR, // "||"
    AST_OP_TERNARY_CONDITIONAL, // "?"
    AST_OP_ASSIGNMENT, // "="
    AST_OP_ADDITION_ASSIGNMENT, // "+="
    AST_OP_SUBTRACTION_ASSIGNMENT, // "-="
    AST_OP_MULTIPLICATION_ASSIGNMENT, // "*="
    AST_OP_DIVISION_ASSIGNMENT, // "/="
    AST_OP_MODULUS_ASSIGNMENT, // "%="
    AST_OP_LEFT_SHIFT_ASSIGNMENT, // "<<="
    AST_OP_RIGHT_SHIFT_ASSIGNMENT, // ">>="
    AST_OP_BITWISE_AND_ASSIGNMENT, // "&="
    AST_OP_BITWISE_XOR_ASSIGNMENT, // "^="
    AST_OP_BITWISE_OR_ASSIGNMENT, // "|="
    AST_OP_COMMA, // ","

    // unary ops.
    AST_OP_PREFIX_INCREMENT, // "++"
    AST_OP_SUFFIX_INCREMENT, // "++"
    AST_OP_PREFIX_DECREMENT, // "--"
    AST_OP_SUFFIX_DECREMENT, // "--"
    AST_OP_LOGICAL_NOT, // "~"
    AST_OP_BITWISE_NOT, // "!"
    AST_OP_DATA_PACK, // "{}"

    AST_OP_SPAN_CTOR, // "..<"
    AST_OP_SPAN_CTOR_STRICT, // "..="

    AST_KEYWORD // TODO: this feels too unreadable as an AST node. what does this mean?
};

#define CASE_AST_OP case AST_OP_MEMBER_SELECTION:\
    case AST_OP_LOGICAL_NOT:\
    case AST_OP_BITWISE_NOT:\
    case AST_OP_FUNCTION_CALL:\
    case AST_OP_ARRAY_SUBSCRIPT:\
    case AST_OP_MULTIPLICATION:\
    case AST_OP_DIVISION:\
    case AST_OP_MODULUS:\
    case AST_OP_ADDITION:\
    case AST_OP_SUBTRACTION:\
    case AST_OP_BITWISE_LEFT_SHIFT:\
    case AST_OP_BITWISE_RIGHT_SHIFT:\
    case AST_OP_LESS_THAN:\
    case AST_OP_LESS_THAN_OR_EQUAL_TO:\
    case AST_OP_GREATER_THAN:\
    case AST_OP_GREATER_THAN_OR_EQUAL_TO:\
    case AST_OP_EQUAL_TO:\
    case AST_OP_NOT_EQUAL_TO:\
    case AST_OP_BITWISE_AND:\
    case AST_OP_BITWISE_XOR:\
    case AST_OP_BITWISE_OR:\
    case AST_OP_LOGICAL_AND:\
    case AST_OP_LOGICAL_OR:\
    case AST_OP_TERNARY_CONDITIONAL:\
    case AST_OP_ASSIGNMENT:\
    case AST_OP_ADDITION_ASSIGNMENT:\
    case AST_OP_SUBTRACTION_ASSIGNMENT:\
    case AST_OP_MULTIPLICATION_ASSIGNMENT:\
    case AST_OP_DIVISION_ASSIGNMENT:\
    case AST_OP_MODULUS_ASSIGNMENT:\
    case AST_OP_LEFT_SHIFT_ASSIGNMENT:\
    case AST_OP_RIGHT_SHIFT_ASSIGNMENT:\
    case AST_OP_BITWISE_AND_ASSIGNMENT:\
    case AST_OP_BITWISE_XOR_ASSIGNMENT:\
    case AST_OP_BITWISE_OR_ASSIGNMENT:\
    case AST_OP_COMMA:\
    case AST_OP_PREFIX_INCREMENT:\
    case AST_OP_SUFFIX_INCREMENT:\
    case AST_OP_PREFIX_DECREMENT:\
    case AST_OP_SUFFIX_DECREMENT:\
    case AST_OP_DATA_PACK:\
    case AST_OP_SPAN_CTOR:\
    case AST_OP_SPAN_CTOR_STRICT:

tree_type TokenToAstOp(token_type tokenOp) {
    switch (tokenOp) {
        case TOKEN_OP_MEMBER_SELECTION: return AST_OP_MEMBER_SELECTION;
        case TOKEN_OP_MULTIPLICATION: return AST_OP_MULTIPLICATION;
        case TOKEN_OP_DIVISION: return AST_OP_DIVISION;
        case TOKEN_OP_MODULUS: return AST_OP_MODULUS;
        case TOKEN_OP_ADDITION: return AST_OP_ADDITION;
        case TOKEN_OP_SUBTRACTION: return AST_OP_SUBTRACTION;
        //case TOKEN_OP_INCREMENT: return AST_OP_INCREMENT;
        //case TOKEN_OP_DECREMENT: return AST_OP_DECREMENT;
        case TOKEN_OP_LOGICAL_NOT: return AST_OP_LOGICAL_NOT;
        case TOKEN_OP_BITWISE_NOT: return AST_OP_BITWISE_NOT;
 
        case TOKEN_OP_SPAN_CTOR: return AST_OP_SPAN_CTOR;
        case TOKEN_OP_SPAN_CTOR_STRICT: return AST_OP_SPAN_CTOR_STRICT;
        case TOKEN_OP_BITWISE_LEFT_SHIFT: return AST_OP_BITWISE_LEFT_SHIFT;
        case TOKEN_OP_BITWISE_RIGHT_SHIFT: return AST_OP_BITWISE_RIGHT_SHIFT;
        case TOKEN_OP_LESS_THAN: return AST_OP_LESS_THAN;
        case TOKEN_OP_LESS_THAN_OR_EQUAL_TO: return AST_OP_LESS_THAN_OR_EQUAL_TO;
        case TOKEN_OP_GREATER_THAN: return AST_OP_GREATER_THAN;
        case TOKEN_OP_GREATER_THAN_OR_EQUAL_TO: return AST_OP_GREATER_THAN_OR_EQUAL_TO;
        case TOKEN_OP_EQUAL_TO: return AST_OP_EQUAL_TO;
        case TOKEN_OP_NOT_EQUAL_TO: return AST_OP_NOT_EQUAL_TO;
        case TOKEN_OP_BITWISE_AND: return AST_OP_BITWISE_AND;
        case TOKEN_OP_BITWISE_XOR: return AST_OP_BITWISE_XOR;
        case TOKEN_OP_BITWISE_OR: return AST_OP_BITWISE_OR;
        case TOKEN_OP_LOGICAL_AND: return AST_OP_LOGICAL_AND;
        case TOKEN_OP_LOGICAL_OR: return AST_OP_LOGICAL_OR;
        case TOKEN_OP_TERNARY_CONDITIONAL: return AST_OP_TERNARY_CONDITIONAL;
        case TOKEN_OP_ASSIGNMENT: return AST_OP_ASSIGNMENT;
        case TOKEN_OP_ADDITION_ASSIGNMENT: return AST_OP_ADDITION_ASSIGNMENT;
        case TOKEN_OP_SUBTRACTION_ASSIGNMENT: return AST_OP_SUBTRACTION_ASSIGNMENT;
        case TOKEN_OP_MULTIPLICATION_ASSIGNMENT: return AST_OP_MULTIPLICATION_ASSIGNMENT;
        case TOKEN_OP_DIVISION_ASSIGNMENT: return AST_OP_DIVISION_ASSIGNMENT;
        case TOKEN_OP_MODULUS_ASSIGNMENT: return AST_OP_MODULUS_ASSIGNMENT;
        case TOKEN_OP_LEFT_SHIFT_ASSIGNMENT: return AST_OP_LEFT_SHIFT_ASSIGNMENT;
        case TOKEN_OP_RIGHT_SHIFT_ASSIGNMENT: return AST_OP_RIGHT_SHIFT_ASSIGNMENT;
        case TOKEN_OP_BITWISE_AND_ASSIGNMENT: return AST_OP_BITWISE_AND_ASSIGNMENT;
        case TOKEN_OP_BITWISE_XOR_ASSIGNMENT: return AST_OP_BITWISE_XOR_ASSIGNMENT;
        case TOKEN_OP_BITWISE_OR_ASSIGNMENT: return AST_OP_BITWISE_OR_ASSIGNMENT;
        case TOKEN_OP_COMMA: return AST_OP_COMMA;
        default:
        // note: you should never call this function unless you know what you are doing.
        // to reach here is what we would consider a bug.
            PPL_TODO;
    }
    return AST_OP_COMMA;
}

const char* astOpToString(tree_type op) {
    switch (op) {
        case AST_OP_MEMBER_SELECTION: return "AST_OP_MEMBER_SELECTION";
        case AST_OP_FUNCTION_CALL: return "AST_OP_FUNCTION_CALL";
        case AST_OP_ARRAY_SUBSCRIPT: return "AST_OP_ARRAY_SUBSCRIPT";
        case AST_OP_MULTIPLICATION: return "AST_OP_MULTIPLICATION";
        case AST_OP_DIVISION: return "AST_OP_DIVISION";
        case AST_OP_MODULUS: return "AST_OP_MODULUS";
        case AST_OP_ADDITION: return "AST_OP_ADDITION";
        case AST_OP_SUBTRACTION: return "AST_OP_SUBTRACTION";
        case AST_OP_BITWISE_LEFT_SHIFT: return "AST_OP_BITWISE_LEFT_SHIFT";
        case AST_OP_BITWISE_RIGHT_SHIFT: return "AST_OP_BITWISE_RIGHT_SHIFT";
        case AST_OP_LESS_THAN: return "AST_OP_LESS_THAN";
        case AST_OP_LESS_THAN_OR_EQUAL_TO: return "AST_OP_LESS_THAN_OR_EQUAL_TO";
        case AST_OP_GREATER_THAN: return "AST_OP_GREATER_THAN";
        case AST_OP_GREATER_THAN_OR_EQUAL_TO: return "AST_OP_GREATER_THAN_OR_EQUAL_TO";
        case AST_OP_EQUAL_TO: return "AST_OP_EQUAL_TO";
        case AST_OP_NOT_EQUAL_TO: return "AST_OP_NOT_EQUAL_TO";
        case AST_OP_BITWISE_AND: return "AST_OP_BITWISE_AND";
        case AST_OP_BITWISE_XOR: return "AST_OP_BITWISE_XOR";
        case AST_OP_BITWISE_OR: return "AST_OP_BITWISE_OR";
        case AST_OP_LOGICAL_AND: return "AST_OP_LOGICAL_AND";
        case AST_OP_LOGICAL_OR: return "AST_OP_LOGICAL_OR";
        case AST_OP_TERNARY_CONDITIONAL: return "AST_OP_TERNARY_CONDITIONAL";
        case AST_OP_ASSIGNMENT: return "AST_OP_ASSIGNMENT";
        case AST_OP_ADDITION_ASSIGNMENT: return "AST_OP_ADDITION_ASSIGNMENT";
        case AST_OP_SUBTRACTION_ASSIGNMENT: return "AST_OP_SUBTRACTION_ASSIGNMENT";
        case AST_OP_MULTIPLICATION_ASSIGNMENT: return "AST_OP_MULTIPLICATION_ASSIGNMENT";
        case AST_OP_DIVISION_ASSIGNMENT: return "AST_OP_DIVISION_ASSIGNMENT";
        case AST_OP_MODULUS_ASSIGNMENT: return "AST_OP_MODULUS_ASSIGNMENT";
        case AST_OP_LEFT_SHIFT_ASSIGNMENT: return "AST_OP_LEFT_SHIFT_ASSIGNMENT";
        case AST_OP_RIGHT_SHIFT_ASSIGNMENT: return "AST_OP_RIGHT_SHIFT_ASSIGNMENT";
        case AST_OP_BITWISE_AND_ASSIGNMENT: return "AST_OP_BITWISE_AND_ASSIGNMENT";
        case AST_OP_BITWISE_XOR_ASSIGNMENT: return "AST_OP_BITWISE_XOR_ASSIGNMENT";
        case AST_OP_BITWISE_OR_ASSIGNMENT: return "AST_OP_BITWISE_OR_ASSIGNMENT";
        case AST_OP_COMMA: return "AST_OP_COMMA";
       // case AST_OP_INCREMENT: return "AST_OP_INCREMENT";
       // case AST_OP_DECREMENT: return "AST_OP_DECREMENT";
        case AST_OP_DATA_PACK: return "AST_OP_DATA_PACK";
        case AST_OP_SPAN_CTOR: return "AST_OP_SPAN_CTOR";
        case AST_OP_SPAN_CTOR_STRICT: return "AST_OP_SPAN_CTOR_STRICT";
        default: 
            PPL_TODO;
            return "<Unknown AST_OP*>";
    }
}

// TODO: it's kind of annoying that we go full circle here. because at one point we indeed had the string
// representation. if we could somehow link the AST to the token or set of tokens that it corresponds to that
// would be quite dope.
const char* astOpToHumanReadableString(tree_type op) {
    switch (op) {
        case AST_OP_MEMBER_SELECTION: return ".";
        case AST_OP_FUNCTION_CALL: return "FUNCTION_CALL";
        case AST_OP_ARRAY_SUBSCRIPT: return "ARRAY_SUBSCRIPT";
        case AST_OP_MULTIPLICATION: return "*";
        case AST_OP_DIVISION: return "/";
        case AST_OP_MODULUS: return "%";
        case AST_OP_ADDITION: return "+";
        case AST_OP_SUBTRACTION: return "-";
        case AST_OP_BITWISE_LEFT_SHIFT: return "<<";
        case AST_OP_BITWISE_RIGHT_SHIFT: return ">>";
        case AST_OP_LESS_THAN: return "<";
        case AST_OP_LESS_THAN_OR_EQUAL_TO: return "<=";
        case AST_OP_GREATER_THAN: return ">";
        case AST_OP_GREATER_THAN_OR_EQUAL_TO: return ">=";
        case AST_OP_EQUAL_TO: return "==";
        case AST_OP_NOT_EQUAL_TO: return "!=";
        case AST_OP_BITWISE_AND: return "&";
        case AST_OP_BITWISE_XOR: return "^";
        case AST_OP_BITWISE_OR: return "|";
        case AST_OP_LOGICAL_AND: return "&&";
        case AST_OP_LOGICAL_OR: return "||";
        case AST_OP_LOGICAL_NOT: return "!";
        case AST_OP_BITWISE_NOT: return "~";
        case AST_OP_TERNARY_CONDITIONAL: return "TERNARY_CONDITIONAL";
        case AST_OP_ASSIGNMENT: return "=";
        case AST_OP_ADDITION_ASSIGNMENT: return "+=";
        case AST_OP_SUBTRACTION_ASSIGNMENT: return "-=";
        case AST_OP_MULTIPLICATION_ASSIGNMENT: return "*=";
        case AST_OP_DIVISION_ASSIGNMENT: return "/=";
        case AST_OP_MODULUS_ASSIGNMENT: return "%=";
        case AST_OP_LEFT_SHIFT_ASSIGNMENT: return "<<=";
        case AST_OP_RIGHT_SHIFT_ASSIGNMENT: return ">>=";
        case AST_OP_BITWISE_AND_ASSIGNMENT: return "&=";
        case AST_OP_BITWISE_XOR_ASSIGNMENT: return "^=";
        case AST_OP_BITWISE_OR_ASSIGNMENT: return "|=";
        case AST_OP_COMMA: return ",";
        //case AST_OP_PREFIX_INCREMENT: return "++";
        //case AST_OP_DECREMENT: return "AST_OP_DECREMENT";
        case AST_OP_DATA_PACK: return "DATA_PACK";
        case AST_OP_SPAN_CTOR: return "..<";
        case AST_OP_SPAN_CTOR_STRICT: return "..=";
        default: 
            PPL_TODO;
            return "<Unknown AST_OP*>";
    }
}

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

bool AstIsOp(const tree_node &tn) {
    switch(tn.type) {
        CASE_AST_OP
        return true;
        default:
        return false;
    }
}

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

// returns true if a and b are the same tree, false otherwise.
bool TreeCompare(struct tree_node &a, struct tree_node b, bool bLog) {
    bool bSame = false;

    if (a.childrenCount == b.childrenCount) {
        if (a.type == b.type) {
            switch(a.type) {
                case AST_INT_LITERAL:
                if ( a.metadata.num == b.metadata.num && a.metadata.valueKind == b.metadata.valueKind ) {
                    bSame = true;
                } else if (bLog) {
                    LOGGER.Error("AST_INT_LITERAL mismatch with (%u,%u) and (%s,%s)",
                        a.metadata.num, b.metadata.num,
                        PplTypeToString(a.metadata.valueKind),PplTypeToString(b.metadata.valueKind));
                }
                break;
                case AST_DECIMAL_LITERAL:
                if ( a.metadata.dnum == b.metadata.dnum && a.metadata.valueKind == b.metadata.valueKind  ) {
                    bSame = true;
                } else if (bLog) {
                    LOGGER.Error("AST_DECIMAL_LITERAL mismatch with (%f,%f) and (%s,%s)",
                        a.metadata.dnum, b.metadata.dnum,
                        PplTypeToString(a.metadata.valueKind),PplTypeToString(b.metadata.valueKind));
                }
                break;
                case AST_KEYWORD: // TODO: keywords should just be enum saying what kind of keyword it is.
                case AST_GNODE:
                case AST_STRING_LITERAL:
                case AST_SYMBOL:
                if (0 == strcmp(a.metadata.str, b.metadata.str)) {
                    bSame = true;
                } else if (bLog) {
                    LOGGER.Error("AST_* mismatch with strings (%s,%s)",
                        a.metadata.str, b.metadata.str);
                }
                break;
                case AST_NULL_LITERAL:
                CASE_AST_OP
                bSame = true;
                break;
            }
        } else if (bLog) {
            LOGGER.Error("type mismatch with (%s,%s)", 
                PplTypeToString(a.metadata.valueKind),PplTypeToString(b.metadata.valueKind));
        }
    } else if (bLog) {
        LOGGER.Error("childrenCount mismatch with (%u,%u)", a.childrenCount, b.childrenCount);
    }

    if (bSame) {
        for (int i = 0;i < a.childrenCount; i++) {
            tree_node aa = a.children[i];
            tree_node bb = b.children[i];
            bSame &= TreeCompare(aa, bb, bLog);
        }
    }

    return bSame;
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
        CASE_AST_OP
            PrintFn("%s%s", sillyBuff, astOpToHumanReadableString(tn.type));
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
