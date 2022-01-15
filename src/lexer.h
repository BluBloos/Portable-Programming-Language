/*
WHAT IS GOING ON: 

Want to refactor the code because I think it's a good practice to write clean
code. Especially if you are doing a code conversion such as you are,
this is the perfect opportunity to refactor at the same time.

One of the things that I thought about while on my run:
- Maybe consider the use of a switch statement and switch on the current character.
- Maybe look up what the standard practice is for writing a lexer. How are these things written?
- Overall I was thinking that a switch statement might make it less readable? 
    Like: The code I have right now is quite readable as it is...this is good!
        Likely just a matter of staring at the code long enough for it to make more
        sense.
*/

char * TYPES[] = {
    "float", "double", "int", "char", "short", "string", "bool", "void"
};

char *KEYWORDS[] = {
    "struct", "continue", "break", "if", "while", "for", 
    "else", "return", "const", "sizeof", "fallthrough"
};

char *P_DIRECTIVES[] = {
    "#include"
};

char *OPS = "+-%*!<>=|&?[].~";

char *COMPOUND_OPS[] = {
    "&&", "||", ">=", "<=", "==", "!=", "->",
    "+=", "-=", "*=", "/=", "%=", "&=", "|=", "++", "--"
};

class PreparseContext { };

enum lexer_state {
    LEXER_NORMAL,
    LEXER_COMMENT,
    LEXER_MULTILINE_COMMENT,
    LEXER_QUOTE
};

// Implements a safe way to pseudo-index into a file.
class RawFileReader {
    public:
    FILE *internalFile;
    unsigned int fileByteCount;
    char *internalBuffer;
    unsigned int buffCharCount;
    RawFileReader(FILE *file) : internalFile(file) {
        fseek(internalFile, 0L, SEEK_END);
        fileByteCount = ftell(internalFile); // ftell is the number of bytes from the beginning of the file.
        fseek(internalFile, 0L, SEEK_SET);
        internalBuffer = (char *)malloc(fileByteCount);
        buffCharCount = 0;
    }
    ~RawFileReader() {
        // NOTE(Noah): I do think destructors and OOP are a nice way for me to do memory management :)
        free(internalBuffer);
    }
    char operator[](int index) {
        // check if index is in possible range.
        if (index < 0 || index >= fileByteCount) return EOF;
        // check if index is not accounted for by internalBuffer
        if ( !(index < buffCharCount) ) {
            while (buffCharCount <= index) {
                char c = fgetc(internalFile);
                internalBuffer[buffCharCount++] = c;
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

class Token {
    public:
    Token() : type(TOKEN_UNDEFINED), line(0), str(NULL) {}
    Token(enum token_type type, std::string str, unsigned int line) : type(type), line(line) {
        this->str = MEMORY_ARENA.StdStringAlloc(str);
    }
    Token(enum token_type type, char *str, unsigned int line ) : type(type), line(line) {
        this->str= MEMORY_ARENA.StringAlloc(str);
    }
    Token(enum token_type type, char c, unsigned int line) : type(type), c(c), line(line) {}
    Token(enum token_type type, unsigned int line) : type(type), line(line), num(0) {}
    Token(enum token_type type, double dnum, unsigned int line) : type(type), dnum(dnum), line(line) {}
    Token(enum token_type type, unsigned int num, unsigned int line) : type(type), num(num), line(line) {}
    Token(const Token &tok) : type(tok.type), line(tok.line) {
        num = tok.num; // Should work for all union members...?
    }
    ~Token() {
        // Do nothing.
    }
    enum token_type type;
    union // 64 bit.
    {
        char *str;
        double dnum;
        uint64 num;
        char c;
    };
    uint32 line; // TODO(Noah): Add support for programs with more than 4 billion lines.
};

class TokenContainer {
    public:
    std::vector<Token> tokens;
    void Append(Token tok) {
        tokens.push_back(tok);
    }
    void Print() {
        for (int i = 0; i < tokens.size(); i++) {
            Token &tok = tokens[i];
            printf("%d, ", tok.line);
            switch(tok.type) {
                case TOKEN_UNDEFINED:
                LOGGER.Min("TOKEN_UNDEFINED");
                break;
                case TOKEN_QUOTE:
                LOGGER.Min("TOKEN_QUOTE: %s", tok.str);
                break;
                case TOKEN_INTEGER_LITERAL:
                LOGGER.Min("TOKEN_INTEGER_LITERAL: %d", tok.num);
                break;
                case TOKEN_DECIMAL_LITERAL:
                LOGGER.Min("TOKEN_DECIMAL_LITERAL: %f", tok.dnum);
                break;
                case TOKEN_CHARACTER_LITERAL:
                LOGGER.Min("TOKEN_CHARACTER_LITERAL: %c", tok.c);
                break;
                case TOKEN_ENDL:
                LOGGER.Min("TOKEN_ENDL");
                break;
                case TOKEN_OP:
                LOGGER.Min("TOKEN_OP: %c", tok.c);
                break;
                case TOKEN_COP:
                LOGGER.Min("TOKEN_COP: %s", tok.str);
                break;
                case TOKEN_PART:
                LOGGER.Min("TOKEN_PART: %c", tok.c);
                break;
                case TOKEN_KEYWORD:
                LOGGER.Min("TOKEN_KEYWORD: %s", tok.str);
                break;
                case TOKEN_PDIRECTIVE:
                LOGGER.Min("TOKEN_PDIRECTIVE: %s", tok.str);
                break;
                case TOKEN_SYMBOL:
                LOGGER.Min("TOKEN_SYMBOL: %s", tok.str);
                break;
            }
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
bool TokenFromLatent(Token &token) {
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
    Token &tok,
    Token &symbolTok
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
    char character,  
    char *test, 
    enum token_type tokType, 
    Token &tok,
    Token &symbolTok
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
    Token &tok,
    Token &symbolTok
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

void CurrentTokenAddChar(char c) {
    *currentToken += c;
    // TODO(Noah): Add other whitespace characters.
    if (c != ' ' && c != '\n') {
        *cleanToken += c;
    }
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

/* 
NOTE(Noah):
The lexer should never fail. It just returns tokens. 
But since we are combining witht the preparser (which opens files),
this might fail.
NOTE(Noah): Was debating on reading in file entirely into RAM, then parsing it.
However, if we read character by character, this opens up the door for future 
parallelizing of this code. This becomes important when the source file is HUGE.
*/
bool LexAndPreparse(
    FILE *inFile, 
    TokenContainer &tokenContainer, 
    PreparseContext &preparseContext
) 
{    

    // Generate globals
    std::string cToken = ""; currentToken = &cToken;
    std::string clToken = ""; cleanToken = &clToken;

    // # for the sake of parsing a raw input of ' '
    // # append onto raw.
    // raw += ' '
    
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
    char character = raw[0]; 
    while (character != EOF) {
        
        n += 1;
        character = raw[n];
        
        // check for newline characters.
        if (character == '\n') {
            currentLine += 1;
            // NOTE(Noah): no 'continue;' because comments exit on newline.  
        }

        // Handle comment, multiline, and quote states.
        if (state == LEXER_COMMENT) {
            if (character == '\n' || character == EOF) // end condition check
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
            // TODO(Noah): More special characters to implement here.
            else if (character == '\\' && raw[n-1] != '\\') {
                if (raw[n+1] != 'n') {
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
        //# TODO(Noah): Make sure that we do not go outside of the range of raw.
        else if (state == LEXER_NORMAL) { 
            
            //# check for single line comments.
            if (character == '/' && raw[n+1] == '/'){
                state = LEXER_COMMENT;
                Token token;
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
                Token token;
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
                Token token;
                if (TokenFromLatent(token)) {
                    tokenContainer.Append(token);
                }
                CurrentTokenReset();
                continue;
            }

            // Check for character literals.
            if (raw[n] == '\'' && raw[n+2] == '\'') {
                char c_val = raw[n+1];
                tokenContainer.Append(Token(TOKEN_CHARACTER_LITERAL, c_val, currentLine));
                CurrentTokenReset();
                n += 2;
                continue;
            }

            // NOTE(Noah): Should we not lex, for example, 100. ? (because right now we do).
                // Upon further inspection we do not...? i.e. IsNumber returns false for this input.
            // consume '.' in decimal literals to avoid being parsed as a TOKEN_PART. 
            bool _df;
            if (raw[n] == '.' && IsNumber(*cleanToken, _df) ) {
                CurrentTokenAddChar('.');
                continue;
            }

            // Check for division statements
            if (character == '/' && raw[n+1] != '/') {
                Token token;
                if (TokenFromLatent(token)) {
                    tokenContainer.Append(token);
                }
                tokenContainer.Append(Token(TOKEN_OP, "/", currentLine));
                CurrentTokenReset();
                continue;
            }

            // We want to check for symbols if we have hit a space character or newline character (some whitespace).
            if (character == ' ' || character == '\n') {
                unsigned int realLine = (character == '\n') ? currentLine - 1  : currentLine;
                Token token;
                if (TokenFromLatent(token)) {
                    tokenContainer.Append(token);
                    CurrentTokenReset();
                    continue;
                }
            }

            // Go through all search patterns.
            bool foundToken = false;
            for (int i = 0; i < sizeof(sPatterns) / sizeof(struct search_pattern); i++) {
                struct search_pattern sPattern = sPatterns[i];
                Token tok;
                Token symbolTok;
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
                        *cleanToken + character,
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