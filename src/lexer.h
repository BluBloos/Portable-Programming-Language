#ifndef LEXER_H // TODO: make this file a .hpp
#define LEXER_H

typedef unsigned int UNICODE_CPOINT;
#define CP_EOF 0

#include "ppl_types.hpp"

void GenerateCodeContextFromFilePos(ppl_error_context &ctx, uint32_t line, uint32_t c, char *buf, uint32_t bufLen);

// keywords that map to values such as:
// "true", "false", "null",
// are mapped to their own TOKEN kind.

char *KEYWORDS[] = {

    "continue", "break", "return",

    "if", "else", "then", "for", "in", "while", "do",

    "switch", "case", "default", "fall",

    "defer",

    "fn", // function types.

    // qualifiers
    "static",
    "unsigned", // TODO: do we really want this?
        
    // TODO: should intrinsics be keywords?
    "size_of",
    "type_of",
    "type_info",
    "offset_of",

    // TODO: is assert a standard lib thing or a compile-time function????
    "assert",

    // TODO: should these be something else? and can I have like the #else, etc versions ???
    "#if",

    // TODO: do we need to move these to actually be ops?
    "#import",
    "#assert", // op
    "#inline"  // op

};

// TODO: maybe there should be a compound part kind of like `->`.
// because `->` isn't really an operator.
char *TOKEN_PARTS = "{}():";

constexpr char ENDLINE_CHAR = ';';

enum lexer_state {
    LEXER_NORMAL,
    LEXER_COMMENT,
    LEXER_MULTILINE_COMMENT,
    LEXER_QUOTE
};

// Implements a safe way to pseudo-index into a file.
class RawFileReader {

public:

    int lastChar; // What was last returned by fgetc.
    FILE *internalFile;
    int fileByteCount;

    // TODO: deal with this.
    UNICODE_CPOINT *internalBuffer = nullptr;
    
    const char *theStupidFile = nullptr; // TODO:

    unsigned int internalBufferSize;
    unsigned int buffCharCount;

    uint32_t *lineInfos = nullptr; // stretchy buffer.

    void AddLineInfo(uint32_t lineInfo)
    {
        StretchyBufferPush(lineInfos, lineInfo);
    }

    ppl_str_view ReadLine(uint32_t line)
    {
        // NOTE: we are using line+1 here because we can expect that there is one extra
        // element in lineInfos so that the length of the last line can be retrieved.

        uint32_t zeroBasedIdx = line-1;

        if ( (line == 0) || ( (zeroBasedIdx+1) >= StretchyBufferCount(lineInfos))) return {};

        uint32_t beginIdx = lineInfos[zeroBasedIdx];
        uint32_t endIdx   = lineInfos[zeroBasedIdx+1];
        ppl_str_view view = {};
        view.str = (char*)&(theStupidFile[beginIdx]);
        view.len = &theStupidFile[endIdx] - &theStupidFile[beginIdx];
        return view;
    }

    // TODO: this is a hack.
    RawFileReader()
    {

    }

    RawFileReader & operator=( RawFileReader &&other )
    {
        lastChar = other.lastChar;
        fileByteCount = other.fileByteCount;
        internalFile = other.internalFile; // we don't own the file.

        std::swap(internalBuffer, other.internalBuffer);
        std::swap(theStupidFile, other.theStupidFile);
        std::swap(lineInfos, other.lineInfos);

        internalBufferSize = other.internalBufferSize;
        buffCharCount = other.buffCharCount;

        return *this;
    }

    RawFileReader &operator=(const RawFileReader &other) = delete;

    RawFileReader(FILE *file) : internalFile(file) {
        if (internalFile != NULL) {
            // get the size of the file.
            int r = fseek(internalFile, 0L, SEEK_END); Assert(r == 0);
            fileByteCount = ftell(internalFile); // ftell is the number of bytes from the beginning of the file.
            Assert(fileByteCount != -1L);
            r = fseek(internalFile, 0L, SEEK_SET); Assert(r == 0); // go back to beginning.

            // TODO: I did this for sanity purposes but we really should think about UTF-8 some more.
            theStupidFile = (const char *)malloc(sizeof(char) * fileByteCount);
            Assert(theStupidFile);
            fread((void*)theStupidFile, sizeof(char), fileByteCount, internalFile);
            r = fseek(internalFile, 0L, SEEK_SET); Assert(r == 0); // go back to beginning.

            // TODO(Noah): This is actually HIGHLY inefficient because we are multiplying the size of files by 4 when
            // representing in internal memory...
            internalBufferSize = fileByteCount;
            internalBuffer = (UNICODE_CPOINT *)malloc(internalBufferSize * sizeof(UNICODE_CPOINT));
            Assert(internalBuffer != NULL);
            buffCharCount = 0;
            lastChar = fgetc(file);
        }
        StretchyBufferInit(lineInfos);
    }
    ~RawFileReader() {
        // NOTE(Noah): I do think destructors and OOP are a nice way for me to do memory management :)
        if (internalBuffer != NULL)
            free(internalBuffer);
        if (lineInfos!=nullptr)
            StretchyBufferFree(lineInfos);
        if (theStupidFile!=nullptr)
           free((void*)theStupidFile);
    }
    UNICODE_CPOINT _fgetucp(FILE *file) {
        // like fgetc, but returns a unicode code point instead.
        // ensure this returns EOF when we call and there is no more file.

        UNICODE_CPOINT cp = 0; 
        int sz = 0; // size;
        
        // there is two problems with this here algorithm.
        // 1. !isutf8 fails because we always start at the beginning of a utf8 character sequence.
        // 2. in order to recognize that we are in fact no longer in a utf8 character sequence,
        // we must call fgetc. But since we fail on this character, we consume it, and it is not
        // able to be recognized into the next character.
        int c = lastChar;
        while (true) {
            if ( c != EOF && c != 0 && (sz == 0 || !isutf(c))  ) {
                cp <<= 6;
                cp += (unsigned char)c; //(*i)++;
                sz++; 
            } else {
                break;
            }

            c = fgetc(file);
            lastChar = c;
        }

        // presuming we hit EOF, but there is a preceding character, this character should be returned.
        // then on the subsequent call to _fgetucp, we see the EOF but cp this time is zero. Now EOF.
        if (c == EOF && cp == 0) { 
            return CP_EOF;
        }

        cp -= offsetsFromUTF8[sz-1];
        return cp;

        /*do {
            cp <<= 6;
            //ch += (unsigned char)s[(*i)++];
            c = fgetc(file);
            cp += (unsigned char)c; (*i)++; 
            sz++;
        } while (s[*i] && !isutf(s[*i])); // previewing the next character.
        ch -= offsetsFromUTF8[sz-1];
        return ch;
        */
    }
    UNICODE_CPOINT operator[](unsigned int index) {
        if (index < 0 ) return CP_EOF;
        // Check if index is not accounted for by internalBuffer
        if ( !(index < buffCharCount) ) {
            while (buffCharCount <= index) {
                if (buffCharCount < internalBufferSize) { 
                    UNICODE_CPOINT c = _fgetucp(internalFile); // this call here 
                    internalBuffer[buffCharCount++] = c;
                } else {
                    return CP_EOF; // overflow for too large indices.
                }
            }
        }
        return internalBuffer[index];
    }
};

enum token_type {
    TOKEN_UNDEFINED,
    TOKEN_QUOTE,
    TOKEN_INTEGER_LITERAL,
    TOKEN_UINT_LITERAL,
    TOKEN_DOUBLE_LITERAL,
    TOKEN_FLOAT_LITERAL,
    TOKEN_CHARACTER_LITERAL,
    TOKEN_TRUE_LITERAL,
    TOKEN_FALSE_LITERAL,
    TOKEN_NULL_LITERAL,
    TOKEN_ENDL,

    TOKEN_OP_MEMBER_SELECTION, // "."
    
    TOKEN_OP_MULTIPLICATION, // "*"
    TOKEN_OP_DIVISION, // "/"
    TOKEN_OP_MODULUS, // "%"
    TOKEN_OP_ADDITION, // "+"
    TOKEN_OP_SUBTRACTION, // "-"

    // unary operators.
    TOKEN_OP_INCREMENT, // "++"
    TOKEN_OP_DECREMENT, // "--"
 
    TOKEN_OP_LOGICAL_NOT, // "!"
    TOKEN_OP_BITWISE_NOT, // "~"

    // range/span? constructor operators.
    TOKEN_OP_SPAN_CTOR, // "..<"
    TOKEN_OP_SPAN_CTOR_STRICT, // "..="
    TOKEN_OP_LEFT_SHIFT_ASSIGNMENT, // "<<="
    TOKEN_OP_RIGHT_SHIFT_ASSIGNMENT, // ">>="
    TOKEN_OP_BITWISE_LEFT_SHIFT, // "<<"
    TOKEN_OP_BITWISE_RIGHT_SHIFT, // ">>"
    TOKEN_OP_LESS_THAN, // "<"
    TOKEN_OP_LESS_THAN_OR_EQUAL_TO, // "<="
    TOKEN_OP_GREATER_THAN, // ">"
    TOKEN_OP_GREATER_THAN_OR_EQUAL_TO, // ">="
    TOKEN_OP_EQUAL_TO, // "=="
    TOKEN_OP_NOT_EQUAL_TO, // "!="
    TOKEN_OP_BITWISE_AND, // "&"
    TOKEN_OP_BITWISE_XOR, // "^"
    TOKEN_OP_BITWISE_OR, // "|"
    TOKEN_OP_LOGICAL_AND, // "&&"
    TOKEN_OP_LOGICAL_OR, // "||"
    TOKEN_OP_TERNARY_CONDITIONAL, // "?"
    TOKEN_OP_ASSIGNMENT, // "="
    TOKEN_OP_ADDITION_ASSIGNMENT, // "+="
    TOKEN_OP_SUBTRACTION_ASSIGNMENT, // "-="
    TOKEN_OP_MULTIPLICATION_ASSIGNMENT, // "*="
    TOKEN_OP_DIVISION_ASSIGNMENT, // "/="
    TOKEN_OP_MODULUS_ASSIGNMENT, // "%="

    TOKEN_OP_DECL_COMPILER, // "::"
    TOKEN_OP_DECL_RUNTIME,  // ":"
    TOKEN_OP_DECL_RUNTIME_INFER, // ":="

    TOKEN_OP_BITWISE_AND_ASSIGNMENT, // "&="
    TOKEN_OP_BITWISE_XOR_ASSIGNMENT, // "^="
    TOKEN_OP_BITWISE_OR_ASSIGNMENT, // "|="
    TOKEN_OP_COMMA, // ","

    TOKEN_PART,
    TOKEN_KEYWORD,
    TOKEN_SYMBOL,

    TOKEN_TYPE_COUNT
};

#define CASE_TOKEN_OP  case TOKEN_OP_MEMBER_SELECTION:\
        case TOKEN_OP_MULTIPLICATION:\
        case TOKEN_OP_DIVISION:\
        case TOKEN_OP_MODULUS:\
        case TOKEN_OP_ADDITION:\
        case TOKEN_OP_SUBTRACTION:\
        case TOKEN_OP_LESS_THAN:\
        case TOKEN_OP_GREATER_THAN:\
        case TOKEN_OP_BITWISE_AND:\
        case TOKEN_OP_BITWISE_XOR:\
        case TOKEN_OP_BITWISE_OR:\
        case TOKEN_OP_ASSIGNMENT:\
        case TOKEN_OP_LOGICAL_NOT:\
        case TOKEN_OP_BITWISE_NOT:\
        case TOKEN_OP_COMMA:\
        case TOKEN_OP_DECL_RUNTIME:

#define CASE_TOKEN_OP_COMPOUND case TOKEN_OP_LOGICAL_AND:\
        case TOKEN_OP_LOGICAL_OR:\
        case TOKEN_OP_GREATER_THAN_OR_EQUAL_TO:\
        case TOKEN_OP_LESS_THAN_OR_EQUAL_TO:\
        case TOKEN_OP_EQUAL_TO:\
        case TOKEN_OP_NOT_EQUAL_TO:\
        case TOKEN_OP_ADDITION_ASSIGNMENT:\
        case TOKEN_OP_SUBTRACTION_ASSIGNMENT:\
        case TOKEN_OP_MULTIPLICATION_ASSIGNMENT:\
        case TOKEN_OP_DIVISION_ASSIGNMENT:\
        case TOKEN_OP_MODULUS_ASSIGNMENT:\
        case TOKEN_OP_BITWISE_AND_ASSIGNMENT:\
        case TOKEN_OP_BITWISE_OR_ASSIGNMENT:\
        case TOKEN_OP_INCREMENT:\
        case TOKEN_OP_DECREMENT:\
        case TOKEN_OP_BITWISE_XOR_ASSIGNMENT:\
        case TOKEN_OP_LEFT_SHIFT_ASSIGNMENT:\
        case TOKEN_OP_RIGHT_SHIFT_ASSIGNMENT:\
        case TOKEN_OP_BITWISE_LEFT_SHIFT:\
        case TOKEN_OP_BITWISE_RIGHT_SHIFT:\
        case TOKEN_OP_SPAN_CTOR_STRICT:\
        case TOKEN_OP_SPAN_CTOR:\
        case TOKEN_OP_DECL_COMPILER:\ 
        case TOKEN_OP_DECL_RUNTIME_INFER:

#define CASE_TOKEN_UNARY_OP case TOKEN_OP_LOGICAL_NOT:\
        case TOKEN_OP_INCREMENT:\
        case TOKEN_OP_DECREMENT:\
        case TOKEN_OP_BITWISE_NOT:

#define CASE_TOKEN_BINARY_OP case TOKEN_OP_LOGICAL_AND:\
        case TOKEN_OP_LOGICAL_OR:\
        case TOKEN_OP_GREATER_THAN_OR_EQUAL_TO:\
        case TOKEN_OP_LESS_THAN_OR_EQUAL_TO:\
        case TOKEN_OP_EQUAL_TO:\
        case TOKEN_OP_NOT_EQUAL_TO:\
        case TOKEN_OP_ADDITION_ASSIGNMENT:\
        case TOKEN_OP_SUBTRACTION_ASSIGNMENT:\
        case TOKEN_OP_MULTIPLICATION_ASSIGNMENT:\
        case TOKEN_OP_DIVISION_ASSIGNMENT:\
        case TOKEN_OP_MODULUS_ASSIGNMENT:\
        case TOKEN_OP_BITWISE_AND_ASSIGNMENT:\
        case TOKEN_OP_BITWISE_OR_ASSIGNMENT:\
        case TOKEN_OP_BITWISE_XOR_ASSIGNMENT:\
        case TOKEN_OP_LEFT_SHIFT_ASSIGNMENT:\
        case TOKEN_OP_RIGHT_SHIFT_ASSIGNMENT:\
        case TOKEN_OP_BITWISE_LEFT_SHIFT:\
        case TOKEN_OP_BITWISE_RIGHT_SHIFT:\
        case TOKEN_OP_SPAN_CTOR_STRICT:\
        case TOKEN_OP_SPAN_CTOR:\
        case TOKEN_OP_MEMBER_SELECTION:\
        case TOKEN_OP_MULTIPLICATION:\
        case TOKEN_OP_DIVISION:\
        case TOKEN_OP_MODULUS:\
        case TOKEN_OP_ADDITION:\
        case TOKEN_OP_SUBTRACTION:\
        case TOKEN_OP_LESS_THAN:\
        case TOKEN_OP_GREATER_THAN:\
        case TOKEN_OP_BITWISE_AND:\
        case TOKEN_OP_BITWISE_XOR:\
        case TOKEN_OP_BITWISE_OR:\
        case TOKEN_OP_ASSIGNMENT:\
        case TOKEN_OP_COMMA:\
        case TOKEN_OP_DECL_COMPILER:\
        case TOKEN_OP_DECL_RUNTIME:\
        case TOKEN_OP_DECL_RUNTIME_INFER:\

bool TokenIsCompoundOp(token_type type) {
    switch(type) {
        CASE_TOKEN_OP_COMPOUND
        return true;
        default:
        return false;
    }
}

// NOTE: Op is not a superclass that contains compound op. op is a separate class.
bool TokenIsOp(token_type type) {
    switch(type) {
        CASE_TOKEN_OP
        return true;
        default:
        return false;
    }
}

const char *StringFromTokenOp(token_type op)
{
    struct {
        token_type  op;
        const char *str;

    } opPropertiesTable[] = {
        {TOKEN_OP_MEMBER_SELECTION, "."},

        {TOKEN_OP_MULTIPLICATION, "*"},
        {TOKEN_OP_DIVISION, "/"},
        {TOKEN_OP_MODULUS, "%"},
        {TOKEN_OP_ADDITION, "+"},
        {TOKEN_OP_SUBTRACTION, "-"},

        {TOKEN_OP_INCREMENT, "++"},
        {TOKEN_OP_DECREMENT, "--"},

        {TOKEN_OP_LOGICAL_NOT, "!"},
        {TOKEN_OP_BITWISE_NOT, "~"},

        {TOKEN_OP_SPAN_CTOR, "..<"},
        {TOKEN_OP_SPAN_CTOR_STRICT, "..="},
        {TOKEN_OP_LEFT_SHIFT_ASSIGNMENT, "<<="},
        {TOKEN_OP_RIGHT_SHIFT_ASSIGNMENT, ">>="},
        {TOKEN_OP_BITWISE_LEFT_SHIFT, "<<"},
        {TOKEN_OP_BITWISE_RIGHT_SHIFT, ">>"},
        {TOKEN_OP_LESS_THAN, "<"},
        {TOKEN_OP_LESS_THAN_OR_EQUAL_TO, "<="},
        {TOKEN_OP_GREATER_THAN, ">"},
        {TOKEN_OP_GREATER_THAN_OR_EQUAL_TO, ">="},
        {TOKEN_OP_EQUAL_TO, "=="},
        {TOKEN_OP_NOT_EQUAL_TO, "!="},
        {TOKEN_OP_BITWISE_AND, "&"},
        {TOKEN_OP_BITWISE_XOR, "^"},
        {TOKEN_OP_BITWISE_OR, "|"},
        {TOKEN_OP_LOGICAL_AND, "&&"},
        {TOKEN_OP_LOGICAL_OR, "||"},
        {TOKEN_OP_TERNARY_CONDITIONAL,
            "?"},  // TODO: there's multiple syntax parts here. better ? is just a part, right?
        {TOKEN_OP_ASSIGNMENT, "="},
        {TOKEN_OP_ADDITION_ASSIGNMENT, "+="},
        {TOKEN_OP_SUBTRACTION_ASSIGNMENT, "-="},
        {TOKEN_OP_MULTIPLICATION_ASSIGNMENT, "*="},
        {TOKEN_OP_DIVISION_ASSIGNMENT, "/="},
        {TOKEN_OP_MODULUS_ASSIGNMENT, "%="},

        {TOKEN_OP_DECL_COMPILER, "::"},
        {TOKEN_OP_DECL_RUNTIME, ":"},
        {TOKEN_OP_DECL_RUNTIME_INFER, ":="},

        {TOKEN_OP_BITWISE_AND_ASSIGNMENT, "&="},
        {TOKEN_OP_BITWISE_XOR_ASSIGNMENT, "^="},
        {TOKEN_OP_BITWISE_OR_ASSIGNMENT, "|="},
        {TOKEN_OP_COMMA, ","},
    };

    auto prop = opPropertiesTable[(int)op - (int)TOKEN_OP_MEMBER_SELECTION];

    Assert(prop.op == op);

    return prop.str;
}

// NOTE(Noah): We make tokens a class because they manage memory.
// But also, we never dealloc that memory anyways, because it's controlled by the memory
// arena which does an automatic free.
struct token {   
    enum token_type type;
    union // 64 bit.
    {
        char *str;
        double dnum;
        uint64 num;
        UNICODE_CPOINT c;
    };
    // TODO(Noah): Add support for programs with more than 4 billion lines.
    uint32 line; 
    uint32 beginCol;
};


struct token Token() {
    struct token t = {};
    t.type = TOKEN_UNDEFINED;
    t.line = 0;
    t.beginCol = 0;
    return t;
}

struct token Token(enum token_type type, unsigned int line, uint32_t bc) {
    struct token t = {};
    t.type = type;
    t.line = line;
    t.beginCol = bc;
    return t;
}

struct token Token(enum token_type type, std::string str, unsigned int line, uint32_t bc) {
    auto t = Token(type, line, bc);
    t.str = MEMORY_ARENA.StdStringAlloc(str);
    return t;
}

struct token Token(enum token_type type, char *str, unsigned int line, uint32_t bc) {
    auto t = Token(type, line, bc);
    t.str = MEMORY_ARENA.StringAlloc(str);
    return t;
}

struct token Token(enum token_type type, UNICODE_CPOINT c, unsigned int line, uint32_t bc) {
    auto t = Token(type, line, bc);
    t.c = c;
    return t;

}

struct token Token(enum token_type type, char c, unsigned int line, uint32_t bc) {
    return Token(type, (UNICODE_CPOINT)c, line, bc); 
}

struct token Token(enum token_type type, double dnum, unsigned int line, uint32_t bc) {
    auto t = Token(type, line, bc);
    t.dnum = dnum;
    return t;
}

struct token Token(enum token_type type, uint64 num, unsigned int line, uint32_t bc) {
    auto t = Token(type, line, bc);
    t.num = num;
    return t;
}

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <algorithm>

// NOTE(Noah): I am unsure if this naming convention matches the rest of everything in this code
// project, but it makes sense because we are defining function for operating on a specific data type.
void TokenPrint(struct token tok)
{
    LOGGER.Min(", line: %u, bc: %u, ", tok.line, tok.beginCol);
    switch (tok.type) {
        case TOKEN_UNDEFINED:
            LOGGER.Min("TOKEN_UNDEFINED\n");
            break;
        case TOKEN_QUOTE: {
            Assert(tok.str != NULL);
            std::string betterStr = std::string(tok.str);
            std::replace(betterStr.begin(), betterStr.end(), ' ', '_');
            LOGGER.Min("TOKEN_QUOTE: %s\n", betterStr.c_str());
        } break;

        // NOTE: we added -Wall so that
        // if any enum is missing from this switch that the compiler screams at us.

        case TOKEN_TRUE_LITERAL:
            LOGGER.Min("TOKEN_TRUE_LITERAL\n");
            break;
        case TOKEN_FALSE_LITERAL:
            LOGGER.Min("TOKEN_FALSE_LITERAL\n");
            break;
        case TOKEN_NULL_LITERAL:
            LOGGER.Min("TOKEN_NULL_LITERAL\n");
            break;
        case TOKEN_UINT_LITERAL:
            LOGGER.Min("TOKEN_UINT_LITERAL: %" PRIu64 "\n", tok.num);
            break;
        case TOKEN_INTEGER_LITERAL:
            LOGGER.Min("TOKEN_INTEGER_LITERAL: %" PRIu64 "\n", tok.num);
            break;
        case TOKEN_DOUBLE_LITERAL:
        // TODO: for doubles we have something else than %f, right?
            LOGGER.Min("TOKEN_DOUBLE_LITERAL: %f\n", tok.dnum);
            break;
        case TOKEN_FLOAT_LITERAL:
            LOGGER.Min("TOKEN_FLOAT_LITERAL: %f\n", tok.dnum);
            break;
        case TOKEN_CHARACTER_LITERAL: {
            char utf8Buff[5];
            u8_toutf8(utf8Buff, 5, &tok.c, 1);
            LOGGER.Min("TOKEN_CHARACTER_LITERAL: %s\n", utf8Buff);
        } break;

        case TOKEN_ENDL:
            LOGGER.Min("TOKEN_ENDL\n");
            break;
        CASE_TOKEN_OP
            {const char *str = StringFromTokenOp( tok.type );
            LOGGER.Min("TOKEN_OP: %s\n", str);
            }break;
        CASE_TOKEN_OP_COMPOUND
            {const char *str = StringFromTokenOp( tok.type );
            LOGGER.Min("TOKEN_COP: %s\n", str);
            }break;
        case TOKEN_PART:
            LOGGER.Min("TOKEN_PART: %c\n", tok.c);
            break;
        case TOKEN_KEYWORD:
            Assert(tok.str != NULL);
            LOGGER.Min("TOKEN_KEYWORD: %s\n", tok.str);
            break;
        case TOKEN_SYMBOL:
            Assert(tok.str != NULL);
            LOGGER.Min("TOKEN_SYMBOL: %s\n", tok.str);
            break;
    }
}

// TODO(Noah): Change the TokenContainer to use StretchyBuffers.
class TokenContainer {
private:
    struct token* tokens;
public:
    TokenContainer() {
        containerSize = 100;
        tokens = (struct token *)malloc(containerSize * sizeof(struct token));
        Assert(tokens != NULL);
        tokenCount = 0;
        _checkpoint = 0;
    }
    ~TokenContainer() {
        if (tokens != NULL) {
            free(tokens);
        }
    }

    // copy-constructor and -assignment.
    TokenContainer(const TokenContainer &other)=delete;
    TokenContainer &operator=(const TokenContainer &other)=delete;
    
    // move-constructor and -assignment.
    TokenContainer(TokenContainer &&other)=delete;
    TokenContainer &operator=(TokenContainer &&other)=delete;
    
    unsigned int tokenCount;
    unsigned int containerSize; // in token count.
    // NOTE(Noah): What if I have many many tokens??? 
    unsigned int _checkpoint;
    
    unsigned int GetSavepoint() { return _checkpoint; }
    void ResetSavepoint(unsigned int check) { 
        _checkpoint = check;
    }
    struct token AdvanceNext() {
        if (_checkpoint < tokenCount) {
            return tokens[_checkpoint++];
        }
        else {
            LOGGER.Error("No more tokens");
            return Token(); // Returns a TOKEN_UNDEFINED.
        }
    }
    struct token QueryNext() {
        if (_checkpoint < tokenCount)
            return tokens[_checkpoint];
        else
            return Token();
    }
    struct token QueryDistance(unsigned int distance) {
        unsigned int i = _checkpoint + distance;
        if (i < tokenCount) {
            return tokens[i];
        } else {
            return Token();
        }
    }
    void Append(struct token tok) {
        // realloc.
        if (tokenCount >= containerSize) 
        {
            containerSize += 100; // TODO: consider more advanced strat here?
            struct token* _tokens = (struct token *)realloc(tokens, containerSize * sizeof(struct token));
            if (_tokens == NULL) {
                // Going to try doing a realloc ourselves.
                struct token* _tokens = (struct token *)malloc(containerSize * sizeof(struct token));
                Assert(_tokens != NULL);
                memcpy(_tokens, tokens, sizeof(struct token) * tokenCount);
                free(tokens);
                tokens = _tokens;
            } else {
                tokens = _tokens;
            }
        }
        // append.
        memcpy(&tokens[tokenCount++], &tok, sizeof(struct token));
    }
    void Print() {
        for (unsigned int i = 0; i < tokenCount; i++) {
            struct token &tok = tokens[i];
            LOGGER.Min("%u: ", i);
            TokenPrint(tok);
        }
    }
};

// Globals local to lexer.cpp ----------------
std::string *currentToken;
std::string *cleanToken;
unsigned int currentLine = 1;

enum lexer_state state = LEXER_NORMAL;
uint32_t stateBeginLine = 0;
uint32_t stateBeginChar = 0;
uint32_t stateBeginCol = 0;

uint32_t n = -1;
uint32_t n_col = 0; // 
// Globals local to lexer.cpp ----------------

constexpr bool EnableIsNumberFlags = true;
constexpr bool DisableIsNumberFlags = false;

template <bool enableFlags=DisableIsNumberFlags>
static bool IsNumber(
    std::string potNum,
    bool *decimalFlag=nullptr, bool *floatFlag=nullptr, bool *unsignedFlag=nullptr)
{
    if (enableFlags)
    {
        *decimalFlag = false;
        *floatFlag   = false;
        *unsignedFlag = false;
    }
    if (potNum == "") return false;
    if (potNum[0] == '.' || potNum[potNum.size() - 1] == '.') return false;
    for (size_t i = 0; i < potNum.size(); i++) {
        char character = potNum[i];
        switch (character) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                break;
            case '.':
                if (enableFlags) *decimalFlag = true;
                break;
            case 'f':
                if (enableFlags) *floatFlag = true;
                if (i != (potNum.size() - 1)) return false;
                break;
            case 'u':
                if (enableFlags) *unsignedFlag = true;
                if (i != (potNum.size() - 1)) return false;
                break;
            break;
            default:
                return false;
        }
    }
    return true;
}

// given str will match a token from the pattern list.
// returns true if the string matched the pattern.
bool TokenFromString(
    std::string str,
    uint32_t bc, // begin col of str in source.
    char **strPattern,
    unsigned int patternLen,
    enum token_type tokType,
    struct token &tok
) {
//    tok = Token();
    for (unsigned int i = 0; i < patternLen; i++) {
        char *mString = strPattern[i];
        char *pStr;
        int k = 0;
        for( pStr = mString; (*pStr != 0 && str[k] == *pStr); (pStr++, k++) );
        if ( (*pStr == 0) && (k == str.size()) ) {
            // Means we made it through entire pattern string and the incoming string.
            tok = Token(tokType, mString, currentLine, bc);
            return true;
        }
    }
    return false;
}

// Checks for a token from a latent currentToken 
// which by definition is a token that is preceding any other token or is preceding whitespace
bool TokenFromLatent(struct token &token) {
    // Latent currentTokens can be literal or symbol tokens
    if (*cleanToken != "") {

        // TODO: this is looking like it might be sus.
        size_t cleanTokenSize = cleanToken->size();
        assert (cleanTokenSize < n_col);
        uint32_t bc = n_col - cleanToken->size();

        bool dFlag; bool fFlag; bool uFlag;
        if (IsNumber<EnableIsNumberFlags>(*cleanToken, &dFlag, &fFlag, &uFlag)) { 
            if (!dFlag) {
                static_assert( sizeof(uint64_t) == sizeof(unsigned long long),
                    "expect sane platform." );
                
                uint64_t num = strtoull(cleanToken->c_str(),
                    nullptr, // endptr. 
                    10);

                if (uFlag)
                    token = Token(TOKEN_UINT_LITERAL, num, currentLine, bc);
                else
                    token = Token(TOKEN_INTEGER_LITERAL, num, currentLine, bc);
            } else if (!fFlag) {
                // TODO: `atof` actually parses the exponent stuff so that sort
                // of thing should be trivial to implement.
                double num = atof(cleanToken->c_str());
                token = Token(TOKEN_DOUBLE_LITERAL, num, currentLine, bc);
            } else {
                // NOTE: atof ignores any invalid things at the end of the string,
                // so `f` is ignored.
                double num = atof(cleanToken->c_str());
                token = Token(TOKEN_FLOAT_LITERAL, num, currentLine, bc);
            }
        }
        // NOTE: the reason that these have dedicated tokens is because
        // we want to lock down what kind of thing these are ASAP.
        // otherwise at some point later we have to ask if the string is equal to e.g. "true".
        else if (*cleanToken == "true") {
            token = Token(TOKEN_TRUE_LITERAL, currentLine, bc);
        }
        else if (*cleanToken == "false") {
            token = Token(TOKEN_FALSE_LITERAL, currentLine, bc);
        }
        else if (*cleanToken == "null") {
            token = Token(TOKEN_NULL_LITERAL, currentLine, bc);
        }
        else {

            // TODO: looks like we can combine these two lists (ppl::TYPE_STRINGS and KEYWORDS).
            if (!TokenFromString(
                    *cleanToken, bc,
                    ppl::TYPE_STRINGS,
                    sizeof(ppl::TYPE_STRINGS) / sizeof(char *),
                    TOKEN_KEYWORD,
                    token
            )) {
                if (!TokenFromString(
                    *cleanToken, bc,
                    KEYWORDS,
                    sizeof(KEYWORDS) / sizeof(char *),
                    TOKEN_KEYWORD,
                    token
                )) {
                    token = Token(TOKEN_SYMBOL, *cleanToken, currentLine, bc);
                }
            }
        }

        return true;
    }
    return false;
}

// Looks ahead to check for any matches with any of the strings.
unsigned int TokenFromLookaheadString(
    RawFileReader &raw,
    char **strPattern,
    unsigned int patternLen,
    enum token_type tokType,
    struct token &tok,
    struct token &symbolTok
) {
    for (unsigned int i = 0; i < patternLen; i++) {
        char *mString = strPattern[i];
        int k = n;
        int j = 0;
        char *pStr;

        // NOTE: the idea here that we compare the unicode codepoint to the char
        // is that our things that we are looking ahead we assume to always be
        // just plain ASCII (and not the extended ascii).

        // TODO: we need to do some sort of a compile-time check on the strings
        // then for sanity that this is indeed the case.

        for (pStr = mString; (raw[k++] == (uint8_t)*pStr && *pStr != 0); pStr++) { j++; }

        if (*pStr == 0) {
            // Means we made it through entire string and matched.
            TokenFromLatent(symbolTok);
            tok = Token(tokType, mString, currentLine, n_col);
            return j;
        }
    }
    return 0;
}

// take current character, the current token but clean.
// also takes a set of test characters.
void TokenFromChar(
    UNICODE_CPOINT character,  
    char *setOfCharactersToTest,
    enum token_type tokType, 
    struct token &tok,
    struct token &symbolTok
) {
    bool charInTest = false;

    // Go thru str till null terminator.
    for (char *pStr = setOfCharactersToTest; *pStr != 0; pStr++) {
        if ((uint8_t)*pStr == character)
            charInTest = true;
    }
    if (charInTest) {
        TokenFromLatent(symbolTok);
        tok = Token(tokType, character, currentLine, n_col);
    }
}

void CurrentTokenReset() {
    *currentToken = "";
    *cleanToken = "";
}

void CurrentTokenAddChar(UNICODE_CPOINT c) {
    char utf8Buff[5];
    u8_toutf8(utf8Buff, 5, &c, 1); 
    currentToken->append(utf8Buff);
    // TODO(Noah): Do other whitspace characters exist?
    if (c != ' ' && c != '\n') {
        cleanToken->append(utf8Buff);
    }
}

std::string StdStringFromStringAndUCP(std::string *str, UNICODE_CPOINT c) {
    std::string s(*str);
    char utf8Buff[5];
    u8_toutf8(utf8Buff, 5, &c, 1); 
    s.append(utf8Buff);
    return s;
}

// Everything is lookahead (lookahead beginning at the current character).
enum search_pattern_type {
    SEARCH_P_CHAR,
    SEARCH_P_STRING,
    SEARCH_P_CURRENT_STRING
};

struct search_pattern {
    enum search_pattern_type sType;
    token_type tokType;
    unsigned int patternLen;
    union {
        char *char_pattern; // patternLen is undefined.
        char **string_pattern; // patternLen is strings in string_pattern.
    };
};

struct search_pattern sPatterns[7];

static unsigned int sPatternsCount=0;

struct search_pattern CreateSearchPattern( enum search_pattern_type sType, enum token_type tokType, char *pattern) {
    struct search_pattern sPattern;
    sPattern.sType = sType;
    sPattern.tokType = tokType;
    sPattern.patternLen = 0;
    sPattern.char_pattern = pattern;
    return sPattern;
}

struct search_pattern CreateSearchPattern( enum search_pattern_type sType, enum token_type tokType, 
char **arr, unsigned int arrSize) {
    struct search_pattern sPattern;
    sPattern.sType = sType;
    sPattern.tokType = tokType;
    sPattern.patternLen = arrSize;
    sPattern.string_pattern = arr;
    return sPattern;
}

bool Lex(
    FILE *inFile, 
    TokenContainer &tokenContainer,
    RawFileReader *pReaderOut,
    ppl_error_context *pErrCtx
) 
{    
    // Generate/reset globals
    std::string cToken = ""; currentToken = &cToken;
    std::string clToken = ""; cleanToken = &clToken;

    currentLine = 1;
    state = LEXER_NORMAL;
    stateBeginLine = 0;
    stateBeginChar = 0;
    stateBeginCol = 0;

    n = -1;
    n_col = 0;

    // # for the sake of parsing a raw input of ' '
    // # append onto raw.
    // raw += ' '
    
    // TODO(Noah): What if reading in the file here fails??
    *pReaderOut = std::move(RawFileReader(inFile));
    RawFileReader &raw = *pReaderOut;
    pErrCtx->pTokenBirthplace = pReaderOut;

    // there is a precedence in any of the searches below where if some things are substrings of patterns,
    // they need to be checked last.

    sPatternsCount = 0;

    sPatterns[sPatternsCount++] = CreateSearchPattern(SEARCH_P_CHAR, TOKEN_PART, TOKEN_PARTS);


    assert( sPatternsCount <= (sizeof(sPatterns) / sizeof(struct search_pattern)) );

    auto changeState = [&](lexer_state s, uint32_t l, uint32_t c, uint32_t cl)
    {
        state = s;
        stateBeginLine = l;
        stateBeginChar = c;
        stateBeginCol = cl;
    };

    // the code below goes through each character one-by-one
    // and uses the program globals `n` and `n_col`.

    UNICODE_CPOINT character = raw[0]; 

    auto advanceChar = [&](uint32_t a)
    {
        n+=a;
        n_col+=a;
    };

    auto advanceLine = [&]()
    {
        currentLine += 1;
        n_col = 0;
        raw.AddLineInfo(n + 1);
    };

    raw.AddLineInfo(0); // for the first line.

    while (character != CP_EOF) {
        
        advanceChar(1);
        character = raw[n];

        const bool shouldAdvanceLine = (character == '\n' || character == CP_EOF);

        // Handle comment, multiline, and quote states.
        if (state == LEXER_COMMENT) {
            if ( shouldAdvanceLine ) // end condition check
            {
                advanceLine();
                changeState(LEXER_NORMAL, currentLine, n + 1, n_col + 1);
            }
            continue;
        } else if (state == LEXER_MULTILINE_COMMENT) {
            if ( shouldAdvanceLine ) advanceLine();
            if (character == '*' && raw[n+1] == '/') { // end condition check
                advanceChar(1);
                changeState(LEXER_NORMAL, currentLine, n + 1, n_col + 1);
            }
            continue;
        }
        // TODO: cursory look at this code makes it feel like quotes will span multiple lines. is that what we desire?
        else if (state == LEXER_QUOTE) {
            if ( shouldAdvanceLine ) advanceLine();
            if (character == '"' && raw[n-1] != '\\') { // end condition check.
                tokenContainer.Append(Token(TOKEN_QUOTE, *currentToken, currentLine, stateBeginCol));
                changeState(LEXER_NORMAL, currentLine, n + 1, n_col + 1);
                CurrentTokenReset();
            }
            // Check for escape sequenced characters (plus special characters).
            // TODO(Noah): Implement the remaining escape sequence characters.
            else if (character == '\\' && raw[n-1] != '\\') {
                if (raw[n+1] == 'n') {
                    CurrentTokenAddChar('\\');
                    CurrentTokenAddChar('n');
                    advanceChar(1);
                }
            } else { // due to else, \\ is nulled out when used as escape character. i.e. does not appear in final string.
                CurrentTokenAddChar(character);
            }
            continue;
        }
        //# supposing that we are in no comment states, be on the lookout to enter one
        else if (state == LEXER_NORMAL) { 
            
            //# check for single line comments.
            if (character == '/' && raw[n+1] == '/'){
                changeState(LEXER_COMMENT, currentLine, n, n_col);
                struct token token;
                if (TokenFromLatent(token)) {
                    tokenContainer.Append(token);
                }
                CurrentTokenReset();
                advanceChar(1);
                continue;
            }
            
            //# check for multi-line comments.
            if (character == '/' && raw[n+1] == '*') {
                changeState(LEXER_MULTILINE_COMMENT, currentLine, n, n_col);
                struct token token;
                if (TokenFromLatent(token)) {
                    tokenContainer.Append(token);
                }
                CurrentTokenReset();
                advanceChar(1);
                continue;
            }

            // Check for beginning of quotes
            if (character == '"') 
            {
                changeState(LEXER_QUOTE, currentLine, n, n_col);
                struct token token;
                if (TokenFromLatent(token)) {
                    tokenContainer.Append(token);
                }
                CurrentTokenReset();
                continue;
            }

            // Check for character literals.
            if (raw[n] == '\'' && raw[n+2] == '\'') {
                UNICODE_CPOINT c_val = raw[n+1];
                tokenContainer.Append(Token(TOKEN_CHARACTER_LITERAL, c_val, currentLine, n_col));
                CurrentTokenReset();
                advanceChar(2);
                continue;
            }
            
            // consume '.' in decimal literals to avoid being parsed as a TOKEN_PART. 
            // must ensure that what comes before the decimal is a number AND what comes after is also a number.
            
            if (raw[n] == '.' && IsNumber(std::string(1, raw[n+1])) && IsNumber(*cleanToken) ) {
                CurrentTokenAddChar('.');
                continue;
            }

            // Check for division statements
            if (character == '/' && raw[n+1] != '/') {
                struct token token;
                if (TokenFromLatent(token)) {
                    tokenContainer.Append(token);
                }
                tokenContainer.Append(Token(TOKEN_OP_DIVISION, '/', currentLine, n_col));
                CurrentTokenReset();
                continue;
            }

            // We want to check for a latent token if we have hit whitespace.
            if (character == ' ' || shouldAdvanceLine ) {

                struct token token;

                bool isTokenLatent = TokenFromLatent(token);
                if (isTokenLatent) {
                    tokenContainer.Append(token);
                    CurrentTokenReset();
                }
                if ( shouldAdvanceLine ) advanceLine();
                
                // TODO: feels like we could always do the continue here ?
                if (isTokenLatent) continue;
            }

            // TODO: I don't really like that the code below was copy pasta from the copy lookahead thing.
            // that feels like the inverse of abstraction, so can we get back to abstraction land?

            // check for compound ops.
            // note that we MUST check for compound ops first, since the single
            // character ops are substrings of the compound ops. we don't want to
            // recognize two single character ops instead of a single character op.
            {
                bool bFoundMatch = false;
                for (unsigned int i = 0; i < TOKEN_TYPE_COUNT; i++) {
                    token_type type = (token_type)i;
                    switch(i) {
                        CASE_TOKEN_OP_COMPOUND
                        {
                            const char *mString = StringFromTokenOp(type);
                            int k = n; // NOTA BENE: n is a global variable - oof.
                            int j = 0;
                            char *pStr;

                            // NOTE: the idea here that we compare the unicode codepoint to the char
                            // is that our things that we are looking ahead we assume to always be
                            // just plain ASCII (and not the extended ascii).

                            // TODO: we need to do some sort of a compile-time check on the strings
                            // then for sanity that this is indeed the case.

                            for (pStr = (char *)mString; (raw[k++] == (uint8_t)*pStr && *pStr != 0); pStr++) { j++; }

                            if (*pStr == 0) {
                                // Means we made it through entire string and matched.
                                struct token symbolTok;
                                if (TokenFromLatent(symbolTok)) tokenContainer.Append(symbolTok);
                                struct token tok = Token( type, currentLine, n_col);
                                tokenContainer.Append(tok);
                                // NOTE: skip by how many characters we look ahead. count j includes current char,
                                // so we do a -1 here to compute the advance amount.
                                advanceChar(j-1);
                                CurrentTokenReset();
                                bFoundMatch = true; break;
                            } 
                        } break;
                        // nothing.
                        // TODO: is there a better way than to go through everything and skip the ones that aren't compound
                        // ops? of course there is.
                    }
                }
                if (bFoundMatch) continue;
            }

            // check for single character ops.
            token_type type = TOKEN_UNDEFINED;
            switch(character) {
                case '.':
                    type = TOKEN_OP_MEMBER_SELECTION;
                    break;
                case '*':
                    type = TOKEN_OP_MULTIPLICATION;
                    break;
                case '/':
                    type = TOKEN_OP_DIVISION;
                    break;
                case '%':
                    type = TOKEN_OP_MODULUS;
                    break;
                case '+':
                    type = TOKEN_OP_ADDITION;
                    break;
                case '-':
                    type = TOKEN_OP_SUBTRACTION;
                    break;
                case '<':
                    type = TOKEN_OP_LESS_THAN;
                    break;
                case '>':
                    type = TOKEN_OP_GREATER_THAN;
                    break;
                case '&':
                    type = TOKEN_OP_BITWISE_AND;
                    break;
                case '^':
                    type = TOKEN_OP_BITWISE_XOR;
                    break;
                case '|':
                    type = TOKEN_OP_BITWISE_OR;
                    break;
                case '=':
                    type = TOKEN_OP_ASSIGNMENT;
                    break;
                case ',':
                    type = TOKEN_OP_COMMA;
                    break;
                case '!':
                    type = TOKEN_OP_LOGICAL_NOT;
                    break;
                case '~':
                    type = TOKEN_OP_BITWISE_NOT;
                    break;
                case ':':
                    type = TOKEN_OP_DECL_RUNTIME;
                    break;
                default:
                    type = TOKEN_UNDEFINED;
                    break;
            }
            if (type != TOKEN_UNDEFINED)
            {
                struct token symbolTok;
                if (TokenFromLatent(symbolTok)) tokenContainer.Append(symbolTok);
                struct token tok = Token(type, currentLine, n_col);
                tokenContainer.Append(tok);
                CurrentTokenReset();
                continue;
            }

            // check for endline.
            if (character == ENDLINE_CHAR) {
                struct token symbolTok;
                if (TokenFromLatent(symbolTok)) tokenContainer.Append(symbolTok);
                struct token tok = Token(TOKEN_ENDL, currentLine, n_col);
                tokenContainer.Append(tok);
                CurrentTokenReset();
                continue;
            }

            // Go through all search patterns. Some search patterns just check the current character.
            // some search patterns are lookahead.
            bool foundToken = false;
            for (size_t i = 0; i < sPatternsCount; i++) {
                struct search_pattern sPattern = sPatterns[i];
                struct token tok = Token();
                struct token symbolTok = Token();
                unsigned int skipAmount = 0;
                switch(sPattern.sType) {
                    case SEARCH_P_CHAR:
                    TokenFromChar(
                        character, 
                        sPattern.char_pattern, 
                        sPattern.tokType, 
                        tok, 
                        symbolTok
                    );
                    break;
                    case SEARCH_P_STRING:
                    skipAmount = TokenFromLookaheadString(
                        raw,
                        sPattern.string_pattern,
                        sPattern.patternLen,
                        sPattern.tokType,
                        tok,
                        symbolTok
                    ) - 1;
                    break;
                    case SEARCH_P_CURRENT_STRING:
                    TokenFromString(
                        StdStringFromStringAndUCP(cleanToken, character),
                        n_col - cleanToken->size(), // beginCol.
                        sPattern.string_pattern,
                        sPattern.patternLen,
                        sPattern.tokType,
                        tok
                    );
                    break;
                }
                if (symbolTok.type != TOKEN_UNDEFINED)
                    tokenContainer.Append(symbolTok);
                // TODO: this if statement here might not be needed. it might be the case that 
                // if symbolTok is not undefined, so is the tok.
                if (tok.type != TOKEN_UNDEFINED) {
                    tokenContainer.Append(tok);
                    advanceChar(skipAmount);
                    foundToken = true;
                    break;
                }
            } // end for loop.
            
            if (!foundToken)
                CurrentTokenAddChar(character);
            else
                CurrentTokenReset();
            
        } // end if state lexer normal or whatever.

    } // end while loop. at this point, we are done lexing.

    if ((state == LEXER_QUOTE) || (state == LEXER_MULTILINE_COMMENT)) {

        uint32_t    c    = stateBeginCol;
        uint32_t    line = stateBeginLine;

        //pErrCtx->c = c;
        //pErrCtx->line = line;

        switch (state) {
            case LEXER_QUOTE: {
                const char *file = LOGGER.logContext.currFile;
                GenerateCodeContextFromFilePos(
                    *pErrCtx, line, c, pErrCtx->codeContext, PPL_ERROR_MESSAGE_MAX_LENGTH);
                LOGGER.EmitUserError(
                    file, line, c, pErrCtx->codeContext, "Unclosed string literal. Began at %d,%d.", line, c);
                return false;
            } break;
            case LEXER_MULTILINE_COMMENT:
            {
                const char *file = LOGGER.logContext.currFile;
                GenerateCodeContextFromFilePos(
                    *pErrCtx, line, c, pErrCtx->codeContext, PPL_ERROR_MESSAGE_MAX_LENGTH);
                LOGGER.EmitUserError(
                    file, line, c, pErrCtx->codeContext, "Unclosed multiline comment. Began at %d,%d.", line, c);
                return false;
            }
                break;
            default:
                if ((state != LEXER_NORMAL) || (state != LEXER_COMMENT)) {
                    assert(false);
                    // TODO(Compiler): did we handle all invalid closing lexer states?
                }
        }
    }

    return true;
}

/* TODO(Noah):
    Next step for PPL on 2021.03.22 is to implement the preparsing stuff.
    So my thought process right now is that we are going to have different 
    containers. One container for the main compilation unit.
    And a container for each import statement.
    We run the lexing and preparsing recursively on each import, so we end up getting a tree
    structure of buckets.

    Each bucket is literally a unit of source code that we are going to want to run 
    through the entire pipeline (grammar generation and so forth).

    At the time of bucket creation we are also going to want to generate some sort of
    context around the buckets (like what is the qualification for this bucket).

    And we will also want to generate context for the #using statements so that we can rename 
    buckets as needed.
*/
struct preparse_context {
    // Need to know how this bucket is going to be qualified.
    char *qualifierKey;
    // Also need to know what it has been renamed to
    char *qualifierKeyOverride;
};

// TODO(Noah): Combine the lexer and the preparser pass. We do 
// not need two seperate things here.
void Preparse(
    TokenContainer &tokenContainer, 
    struct tree_node &tn
) {
    // So this preparser takes in the tokenContainer from the prior lexed
    // inFile.
    // 
    // T as output we write the program "bucket" into the tree node ref
    // provided.
    //
    // This is gonna include the preparse_context as we have written it right now.
    //
    struct token tok;
    for (int i = 0; (tok = tokenContainer.QueryDistance(i)).type != TOKEN_UNDEFINED; i++) {
        TokenPrint(tok);
    }

}


#endif