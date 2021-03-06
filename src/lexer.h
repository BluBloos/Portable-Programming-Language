#ifndef LEXER_H
#define LEXER_H
typedef unsigned int UNICODE_CPOINT;
#define CP_EOF 0

char * TYPES[] = {
    "float", "double", "int", "char", "short", "string", "bool", "void"
};

char *KEYWORDS[] = {
    "struct", "continue", "break", "if", "while", "for", 
    "else", "return", "const", "sizeof", "fallthrough", "switch", 
    "case", "default", "as"
};

char *P_DIRECTIVES[] = {
    "#include", "#import"
};

char *OPS = "+-%*!<>=|&?[].~";

char *COMPOUND_OPS[] = {
    "&&", "||", ">=", "<=", "==", "!=", "->",
    "+=", "-=", "*=", "/=", "%=", "&=", "|=", "++", "--"
};

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
    UNICODE_CPOINT *internalBuffer;
    unsigned int internalBufferSize;
    unsigned int buffCharCount;
    RawFileReader(FILE *file) : internalFile(file) {
        if (internalFile != NULL) {
            int r = fseek(internalFile, 0L, SEEK_END); Assert(r == 0);
            fileByteCount = ftell(internalFile); // ftell is the number of bytes from the beginning of the file.
            Assert(fileByteCount != -1L);
            r = fseek(internalFile, 0L, SEEK_SET); Assert(r == 0);
            // TODO(Noah): This is actually HIGHLY inefficient because we are multiplying the size of files by 4 when
            // representing in internal memory...
            internalBufferSize = fileByteCount;
            internalBuffer = (UNICODE_CPOINT *)malloc(internalBufferSize * sizeof(UNICODE_CPOINT));
            Assert(internalBuffer != NULL);
            buffCharCount = 0;
            lastChar = fgetc(file);
        }
    }
    ~RawFileReader() {
        // NOTE(Noah): I do think destructors and OOP are a nice way for me to do memory management :)
        if (internalBuffer != NULL)
            free(internalBuffer);
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
    UNICODE_CPOINT operator[](int index) {
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
    TOKEN_DECIMAL_LITERAL,
    TOKEN_CHARACTER_LITERAL,
    TOKEN_ENDL,
    TOKEN_OP,
    TOKEN_COP, // compouned op.
    TOKEN_PART,
    TOKEN_KEYWORD,
    TOKEN_PDIRECTIVE, // preprocessor directives.
    TOKEN_SYMBOL
};

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
};
struct token Token() {
    struct token t; t.type = TOKEN_UNDEFINED; t.line = 0; t.str = NULL;
    return t;
}
struct token Token(enum token_type type, std::string str, unsigned int line) {
    struct token t;
    t.str = MEMORY_ARENA.StdStringAlloc(str);
    t.type = type; t.line = line;
    return t;
}
struct token Token(enum token_type type, char *str, unsigned int line ) {
    struct token t;
    t.str = MEMORY_ARENA.StringAlloc(str);
    t.type = type; t.line = line;
    return t;
}
struct token Token(enum token_type type, UNICODE_CPOINT c, unsigned int line) {
    struct token t; t.c = c; t.type = type; t.line = line;
    return t;
}
struct token Token(enum token_type type, char c, unsigned int line) {
    return Token(type, (UNICODE_CPOINT)c, line); 
}
struct token Token(enum token_type type, unsigned int line) {
    struct token t; t.num = 0; t.type = type; t.line = line;
    return t;
}
struct token Token(enum token_type type, double dnum, unsigned int line) {
    struct token t; t.dnum = dnum; t.type = type; t.line = line;
    return t;
}
struct token Token(enum token_type type, uint64 num, unsigned int line) {
    struct token t; t.num = num; t.type = type; t.line = line;
    return t;
}

// NOTE(Noah): I am unsure if this naming convention matches the rest of everything in this code
// project, but it makes sense because we are defining function for operating on a specific data type.
void TokenPrint(struct token tok) {
    LOGGER.Min("%d, ", tok.line);
    switch(tok.type) {
        case TOKEN_UNDEFINED:
        LOGGER.Min("TOKEN_UNDEFINED\n");
        break;
        case TOKEN_QUOTE:
        Assert(tok.str != NULL);
        LOGGER.Min("TOKEN_QUOTE: %s\n", tok.str);
        break;
        case TOKEN_INTEGER_LITERAL:
        LOGGER.Min("TOKEN_INTEGER_LITERAL: %d\n", tok.num);
        break;
        case TOKEN_DECIMAL_LITERAL:
        LOGGER.Min("TOKEN_DECIMAL_LITERAL: %f\n", tok.dnum);
        break;
        case TOKEN_CHARACTER_LITERAL:
        {
            char utf8Buff[5];
            u8_toutf8(utf8Buff, 5, &tok.c, 1);
            LOGGER.Min("TOKEN_CHARACTER_LITERAL: %s\n", utf8Buff);
        }
        break;
        case TOKEN_ENDL:
        LOGGER.Min("TOKEN_ENDL\n");
        break;
        case TOKEN_OP:
        LOGGER.Min("TOKEN_OP: %c\n", tok.c);
        break;
        case TOKEN_COP:
        Assert(tok.str != NULL);
        LOGGER.Min("TOKEN_COP: %s\n", tok.str);
        break;
        case TOKEN_PART:
        LOGGER.Min("TOKEN_PART: %c\n", tok.c);
        break;
        case TOKEN_KEYWORD:
        Assert(tok.str != NULL);
        LOGGER.Min("TOKEN_KEYWORD: %s\n", tok.str);
        break;
        case TOKEN_PDIRECTIVE:
        Assert(tok.str != NULL);
        LOGGER.Min("TOKEN_PDIRECTIVE: %s\n", tok.str);
        break;
        case TOKEN_SYMBOL:
        Assert(tok.str != NULL);
        LOGGER.Min("TOKEN_SYMBOL: %s\n", tok.str);
        break;
    }
}

// TODO(Noah): Change the TokenContainer to use StretchyBuffers.
class TokenContainer {
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
    struct token* tokens;
    unsigned int tokenCount;
    unsigned int containerSize; // in token count.
    // NOTE(Noah): What if I have many many tokens??? 
    unsigned int _checkpoint;
    unsigned int GetSavepoint() { return _checkpoint; }
    void ResetSavepoint(unsigned int check) { 
        _checkpoint = check;
    }
    struct token Next() {
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
        if (tokenCount < containerSize) {
            //tokens[tokenCount++] = tok; // invokes copy constructor.
            memcpy(&tokens[tokenCount++], &tok, sizeof(struct token));
        } else {
            containerSize += 100;
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
    }
    void Print() {
        for (int i = 0; i < tokenCount; i++) {
            struct token &tok = tokens[i];
            TokenPrint(tok);
        }
    }
};

// Globals local to lexer.cpp
std::string *currentToken;
std::string *cleanToken;
unsigned int currentLine = 1;

bool IsNumber(std::string potNum, bool &decimalFlag) {
    if (potNum == "") 
        return false;
    if (potNum[0] == '.' || potNum[potNum.size() - 1] == '.')
        return false;
    for (int i = 0; i < potNum.size(); i++) {
        char character = potNum[i];
        switch(character) {
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
            decimalFlag = true;
            break;
            default:
            return false;
        }
    } 
    return true;
}

// Checks for a token from a latent currentToken 
// which by definition is a token that is preceding any other token or is preceding whitespace
bool TokenFromLatent(struct token &token) {
    // Latent currentTokens can be literal or symbol tokens
    if (*cleanToken != "") { 
        bool dFlag;
        if (IsNumber(*cleanToken, dFlag)) { 
            if (!dFlag) {
                unsigned int num = atoi(cleanToken->c_str());
                token = Token(TOKEN_INTEGER_LITERAL, num, currentLine);
            } else {
                double num = atof(cleanToken->c_str());
                token = Token(TOKEN_DECIMAL_LITERAL, num, currentLine);
            }
        }
        else if (*cleanToken == "true")
            token = Token(TOKEN_INTEGER_LITERAL, (unsigned int)1, currentLine);
        else if (*cleanToken == "false")
            token = Token(TOKEN_INTEGER_LITERAL, (unsigned int)0, currentLine);
        else
            token = Token(TOKEN_SYMBOL, *cleanToken, currentLine);
        return true;
    }
    return false;
}

// Looks ahead to check for any matches with any of the strings.
unsigned int TokenFromLookaheadString(
    RawFileReader &raw,
    unsigned int n,
    char **strPattern,
    unsigned int patternLen,
    enum token_type tokType,
    struct token &tok,
    struct token &symbolTok
) {
    for (int i = 0; i < patternLen; i++) {
        char *mString = strPattern[i];
        int k = n;
        int j = 0;
        char *pStr;
        for( pStr = mString; (raw[k++] == *pStr && *pStr != 0); pStr++ ) {
            j++;
        }
        if (*pStr == 0) {
            // Means we made it through entire string and matched.
            TokenFromLatent(symbolTok);
            tok = Token(tokType, mString, currentLine);
            return j;
        }
    }
    return 0;
}

// take current character, the current token but clean.
// also takes a set of test characters.
void TokenFromChar(
    UNICODE_CPOINT character,  
    char *test, 
    enum token_type tokType, 
    struct token &tok,
    struct token &symbolTok
) {
    bool charInTest = false;
    for (char *pStr = test; *pStr != 0; pStr++) { // Go thru str till null terminator.
        if (*pStr == character)
            charInTest = true;
    }
    if (charInTest) {
        TokenFromLatent(symbolTok);
        tok = Token(tokType, character, currentLine);
    }
}

// given str will match a token from the pattern list.
void TokenFromString(
    std::string str,
    char **strPattern,
    unsigned int patternLen,
    enum token_type tokType,
    struct token &tok,
    struct token &symbolTok
) {
    for (int i = 0; i < patternLen; i++) {
        char *mString = strPattern[i];
        char *pStr;
        int k = 0;
        for( pStr = mString; (*pStr != 0 && str[k] == *pStr); (pStr++, k++) );
        if (*pStr == 0) {
            // Means we made it through entire string and matched.
            // TokenFromLatent(symbolTok);
            tok = Token(tokType, mString, currentLine);
        }
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
    TokenContainer &tokenContainer
) 
{    
    // Generate globals
    std::string cToken = ""; currentToken = &cToken;
    std::string clToken = ""; cleanToken = &clToken;
    currentLine = 1;

    // # for the sake of parsing a raw input of ' '
    // # append onto raw.
    // raw += ' '
    
    // TODO(Noah): What if reading in the file here fails??
    RawFileReader raw = RawFileReader(inFile);

    enum lexer_state state = LEXER_NORMAL;

    sPatterns[0] = CreateSearchPattern(
        SEARCH_P_STRING, TOKEN_COP, COMPOUND_OPS, sizeof(COMPOUND_OPS) / sizeof(char*));
    sPatterns[1] = CreateSearchPattern(SEARCH_P_CHAR, TOKEN_OP, OPS);
    sPatterns[2] = CreateSearchPattern(SEARCH_P_CHAR, TOKEN_ENDL, ";");
    sPatterns[3] = CreateSearchPattern(SEARCH_P_CHAR, TOKEN_PART, "{}(),:");
    sPatterns[4] = CreateSearchPattern(
        SEARCH_P_CURRENT_STRING, TOKEN_KEYWORD, KEYWORDS, sizeof(KEYWORDS) / sizeof(char*));
    sPatterns[5] = CreateSearchPattern(
        SEARCH_P_STRING, TOKEN_PDIRECTIVE, P_DIRECTIVES, sizeof(P_DIRECTIVES) / sizeof(char *)
    );
    sPatterns[6] = CreateSearchPattern(
        SEARCH_P_CURRENT_STRING, TOKEN_KEYWORD, TYPES, sizeof(TYPES) / sizeof(char *)
    );

    // Go through each character one-by-one
    int n = -1;
    UNICODE_CPOINT character = raw[0]; 
    while (character != CP_EOF) {
        
        n += 1;
        character = raw[n];
        
        // check for newline characters.
        if (character == '\n') {
            currentLine += 1;
            // NOTE(Noah): no 'continue;' because comments exit on newline.  
        }

        // Handle comment, multiline, and quote states.
        if (state == LEXER_COMMENT) {
            if (character == '\n' || character == CP_EOF) // end condition check
                state = LEXER_NORMAL;
            continue;
        } else if (state == LEXER_MULTILINE_COMMENT) {
            if (character == '*' && raw[n+1] == '/') { // end condition check
                state = LEXER_NORMAL;
                n += 1;
            }
            continue;
        }
        else if (state == LEXER_QUOTE) {
            if (character == '"' && raw[n-1] != '\\') { // end condition check.
                state = LEXER_NORMAL;
                tokenContainer.Append(Token(TOKEN_QUOTE, *currentToken, currentLine));
                CurrentTokenReset();
            }
            // Check for escape sequenced characters (plus special characters).
            // TODO(Noah): Implement the remaining escape sequence characters.
            else if (character == '\\' && raw[n-1] != '\\') {
                if (raw[n+1] == 'n') {
                    CurrentTokenAddChar('\\');
                    CurrentTokenAddChar('n');
                    n += 1;
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
                state = LEXER_COMMENT;
                struct token token;
                if (TokenFromLatent(token)) {
                    tokenContainer.Append(token);
                }
                CurrentTokenReset();
                n += 1;
                continue;
            }
            
            //# check for multi-line comments.
            if (character == '/' && raw[n+1] == '*') {
                state = LEXER_MULTILINE_COMMENT;
                struct token token;
                if (TokenFromLatent(token)) {
                    tokenContainer.Append(token);
                }
                CurrentTokenReset();
                n += 1;
                continue;
            }

            // Check for beginning of quotes
            if (character == '"') 
            {
                state = LEXER_QUOTE;
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
                tokenContainer.Append(Token(TOKEN_CHARACTER_LITERAL, c_val, currentLine));
                CurrentTokenReset();
                n += 2;
                continue;
            }
            
            // consume '.' in decimal literals to avoid being parsed as a TOKEN_PART. 
            // must ensure that what comes before the decimal is a number AND what comes after is also a number.
            bool _df;
            if (raw[n] == '.' && IsNumber(std::string(1, raw[n+1]), _df) && IsNumber(*cleanToken, _df) ) {
                CurrentTokenAddChar('.');
                continue;
            }

            // Check for division statements
            if (character == '/' && raw[n+1] != '/') {
                struct token token;
                if (TokenFromLatent(token)) {
                    tokenContainer.Append(token);
                }
                tokenContainer.Append(Token(TOKEN_OP, '/', currentLine));
                CurrentTokenReset();
                continue;
            }

            // We want to check for symbols if we have hit whitespace.
            if (character == ' ' || character == '\n' || character == CP_EOF) {
                currentLine = (character == '\n') ? currentLine - 1  : currentLine;
                struct token token;
                bool isTokenLatent = TokenFromLatent(token);
                if (isTokenLatent) {
                    tokenContainer.Append(token);
                    CurrentTokenReset();
                }
                currentLine = (character == '\n') ? currentLine + 1  : currentLine;
                if (isTokenLatent) continue;
            }

            // Go through all search patterns.
            bool foundToken = false;
            for (int i = 0; i < sizeof(sPatterns) / sizeof(struct search_pattern); i++) {
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
                        n,
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
                        sPattern.string_pattern,
                        sPattern.patternLen,
                        sPattern.tokType,
                        tok,
                        symbolTok
                    );
                    break;
                }
                if (symbolTok.type != TOKEN_UNDEFINED)
                    tokenContainer.Append(symbolTok);
                if (tok.type != TOKEN_UNDEFINED) {
                    tokenContainer.Append(tok);
                    n += skipAmount;
                    foundToken = true;
                    break;
                }
            }
            
            if (!foundToken)
                CurrentTokenAddChar(character);
            else
                CurrentTokenReset();
            
        } 

    } 

    return true;
}

/* TODO(Noah):
    Next step for PPL on 2020.03.22 is to implement the preparsing stuff.
    So my thought process right now is that we are going to have different 
    containers. One container for the main compilation unit.
    And a container for each import statement.
    We run the lexing and preparsing recursively on each import, so we end up getting a tree
    structure of buckets.

    Each bucket is literally a unit of source code that we are going to want to run 
    through the entire pipeline (grammer generation and so forth).

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