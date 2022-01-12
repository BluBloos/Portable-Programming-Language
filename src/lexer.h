/*

If we are thinking about how the lexer works. There are various search patterns.
    - search through a string, where each thing in string is a potential character.
        - matching a character as part of a character set.
    - search through a list of strings for a match.
        - matching a string as part of a string set.

*/

// TODO:
    // Implement RawFileRead [] operator overload.

char * TYPES[] = {
    "float", "double", "int", "char", "short", "string", "bool", "void"
};
char *KEYWORDS[] = {
    "struct", "continue", "break", "if", "while", "for", 
    "else", "return", "const", "sizeof", "fallthrough"
};
char *OPS = "+-%*!<>=|&?[].~";
char *COMPOUND_OPS[] = {
    "&&", "||", ">=", "<=", "==", "!=", "->",
    "+=", "-=", "*=", "/=", "%=", "&=", "|=", "++", "--"
};

class PreparseContext {

};

enum lexer_state {
    LEXER_NORMAL,
    LEXER_COMMENT,
    LEXER_MULTILINE_COMMENT,
    LEXER_QUOTE
};

class RawFileReader {
    public:
    FILE *internalFile;
    RawFileReader(FILE *file) : internalFile(file) {}
    char operator[](unsigned int index) {
        // Do something intelligent.
    }
};

enum token_type {
    TOKEN_UNDEFINED,
    TOKEN_QUOTE,
    TOKEN_DECIMAL_LITERAL,
    TOKEN_CHARACTER_LITERAL,
    TOKEN_ENDL,
    TOKEN_OP,
    TOKEN_PART,
    TOKEN_KEYWORD,
    TOKEN_PDIRECTIVE,
    TOKEN_SYMBOL
};

class Token {
    public:
    Token() : type(TOKEN_UNDEFINED), line(0), str(NULL) {}
    Token(enum token_type type, std::string str, unsigned int line) 
        : type(type), line(line) {
        this->str = MEMORY_ARENA.StdStringAlloc(str);
    }
    Token(enum token_type type, char *str, unsigned int line )
        : type(type), line(line) {
            this->str= MEMORY_ARENA.StringAlloc(str);
        }
    Token(enum token_type type, char c, unsigned int line) : type(type), c(c), line(line) {}
    Token(enum token_type type, unsigned int line) : type(type), line(line), num(0) {}
    enum token_type type;
    union // 64 bit.
    {
        char *str;
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
            //struct token tok = tokens[i];
            /*switch(tok.tokenType) {
                 
            }*/
        }
    }
};



bool IsCompoundOp(
    Token &tok, 
    RawFileReader &raw, 
    unsigned int n, 
    unsigned int cLine
) {
    
    // NOTE(Noah): Can use a constant of 2 cuz all compounds ops have len of 2
    char buffer[3] = {
        raw[n], raw[n+1], 0
    };
    int compOpCount = sizeof(COMPOUND_OPS) / sizeof(char *);
    bool isCompoundOp = false;
    for (int i = 0; i < compOpCount; i++) {
        char *op = COMPOUND_OPS[i];
        if ( buffer[0] == op[0] && buffer[1] == op[1] )
            isCompoundOp = true;
    }
    if (isCompoundOp) {
        tok = Token(TOKEN_OP, buffer, cLine);
    }
    return isCompoundOp;
}

bool IsKeyword(
    std::string cleanToken, unsigned cLine, Token &tok) {
    bool isKeyword = false;
    unsigned int numKeyWords = sizeof(KEYWORDS) / sizeof(char *);
    for (int i = 0; i < numKeyWords; i++) {
        if (cleanToken == std::string(KEYWORDS[i]))
            isKeyword = true;
    }
    if (isKeyword) {
        tok = Token(TOKEN_KEYWORD, cleanToken, cLine);
    }
    return isKeyword;
}

// Globals local to lexer.cpp
INTERNAL std::string currentToken = "";
INTERNAL std::string cleanToken = "";
INTERNAL unsigned int currentLine = 0;

bool QueryForSymbolToken(Token &token) {
    return false;
}

// take current character, the current token but clean.
// also takes a set of test characters.
void QueryForCharToken(
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
        QueryForSymbolToken(symbolTok);
        tok = Token(tokType, character, currentLine);
    }
}

void CurrentTokenReset() {
    currentToken = "";
    cleanToken = "";
}

void CurrentTokenAddChar(char c) {
    currentToken += c;
    // TODO(Noah): Add other whitespace characters.
    if (c != ' ' && c != '\n') {
        cleanToken += c;
    }
}

bool IsNumber(std::string potNum) {
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
            case '.':
            break;
            default:
            return false;
        }
    } 
    return true;
}

enum search_pattern_type {
    SEARCH_P_CHAR,
    SEARCH_P_STRING,
    SEARCH_P_STRING_LOOKAHEAD
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

struct search_pattern sPatterns[3];

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
    // # for the sake of parsing a raw input of ' '
    // # append onto raw.
    // raw += ' '
    
    RawFileReader raw = RawFileReader(inFile);
    enum lexer_state state = LEXER_NORMAL;

    sPatterns[0] = CreateSearchPattern(
        SEARCH_P_STRING_LOOKAHEAD, TOKEN_OP, COMPOUND_OPS, sizeof(COMPOUND_OPS) / sizeof(char*));
    sPatterns[1] = CreateSearchPattern(SEARCH_P_CHAR, TOKEN_OP, OPS);
    sPatterns[2] = CreateSearchPattern(SEARCH_P_CHAR, TOKEN_ENDL, ";");
    sPatterns[3] = CreateSearchPattern(SEARCH_P_CHAR, TOKEN_PART, "{}(),:");

    // Go through each character one-by-one
    int n = -1;
    char character = raw[0]; 
    while (character != EOF) {
        
        n += 1;
        character = raw[n];
        bool foundToken = false;
        
        // check for newline characters.
        if (character == '\n') {
            currentLine += 1;
            // NOTE(Noah): We cannot 'continue;' here because comments exit on newline.
            // Also, newlines are used to find the end of SYMBOLS.  
        }

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
                tokenContainer.Append(Token(TOKEN_QUOTE, currentToken, currentLine));
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
                if (QueryForSymbolToken(token)) {
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
                if (QueryForSymbolToken(token)) {
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
                if (QueryForSymbolToken(token, cleanToken, currentLine)) {
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

            // TODO/NOTE(Noah): Should we not lex, for example, 100. ? (because right now we do).
            // consume '.' in decimal literals to avoid being parsed as a TOKEN_PART. 
            if (raw[n] == '.' && IsNumber(cleanToken) ) {
                CurrentTokenAddChar('.');
                continue;
            }

            // Check for preprocessor statements
            // TODO(Noah): Process on the spot.
            {
                Token tok;
                Token symbolTok; 
                QueryForToken(
                    character, cleanToken, "#", TOKEN_PDIRECTIVE, currentLine,
                    tok, symbolTok);
                if (symbolTok.type != TOKEN_UNDEFINED)
                    tokenContainer.Append(symbolTok);
                if (tok.type != TOKEN_UNDEFINED) {
                    // Need to search until a space because prepocessor statements might be #include, #define, etc. 
                    // length, str = searchUntil(raw, n, " ")
                    // tokens.append(Token("PRE", str, currentLine))
                    // currentToken = ""
                    // n += 1 + length
                    // continue
                }
            }

            // Check for division statements
            if (character == '/' && raw[n+1] != '/') {
                Token token;
                if (QueryForSymbolToken(token, cleanToken, currentLine)) {
                    tokenContainer.Append(token);
                }
                tokenContainer.Append(Token(TOKEN_OP, "/", currentLine));
                CurrentTokenReset();
                continue;
            }

            // Check for any compound operators.
            /*Token tok;
            if (IsCompoundOp(tok, raw, n, currentLine)) {
                Token token;
                if (QueryForSymbolToken(token, cleanToken, currentLine)) {
                    tokenContainer.Append(token);
                }
                // NOTE(Noah): We must do a continue here because there are some single character
                    // ops that are a substring of some compound ops.
                tokenContainer.Append(tok);
                CurrentTokenReset();
                n += 1; // NOTE(Noah): All compound ops are of len 2.
                continue;
            }*/

            // Go through all search patterns.
            for (int i = 0; i < 3; i++) {
                struct search_pattern sPattern = sPatterns[i];
                Token tok;
                Token symbolTok;
                unsigned int skipAmount = 0;
                switch(sPattern.sType) {
                    case SEARCH_P_CHAR:
                    QueryForCharToken(
                        character, 
                        sPattern.pattern, 
                        sPattern.tokType, 
                        tok, 
                        symbolTok
                    );
                    break;
                    case SEARCH_P_STRING:
                    /*QueryForStringToken(
                        cleanToken + std::string(character),



                    );*/
                    break;
                    case SEARCH_P_STRING_LOOKAHEAD:
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

            // Recognize keywords
            // TODO(Noah): If I were to say int0, I would still recognize the int and 0 seperate
            {
                Token tok;
                if (IsKeyword(cleanToken, currentLine, tok)) {
                    tokenContainer.Append(tok);
                    foundToken = true;
                }
            }
            
            // We want to check for symbols if we have hit a space character or newline character (some whitespace).
            if (character == ' ' || character == '\n') {
                unsigned int realLine = (character == '\n') ? currentLine - 1  : currentLine;
                Token token;
                if (QueryForSymbolToken(token, cleanToken, currentLine)) {
                    tokenContainer.Append(token);
                    foundToken = true;
                }
            }

            if (!foundToken) {
                CurrentTokenAddChar(character);
            } else {
                CurrentTokenReset();
            }
            
        } 

    } 

    return false;
}