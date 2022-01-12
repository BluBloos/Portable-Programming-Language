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

bool QueryForSymbolToken(Token &token, std::string cToken, unsigned int currentLine) {
    return false;
}

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

// take current character, the current token but clean.
// also takes a set of test characters.
void QueryForToken(
    char character, 
    std::string cleanToken, 
    char *test, 
    enum token_type tokType, 
    unsigned int cLine,
    Token &tok,
    Token &symbolTok
) {
    bool charInTest = false;
    for (char *pStr = test; *pStr != 0; pStr++) { // Go thru str till null terminator.
        if (*pStr == character)
            charInTest = true;
    }
    if (charInTest) {
        QueryForSymbolToken(symbolTok, cleanToken, cLine);
        tok = Token(tokType, character, cLine);
    }
}

// Globals local to lexer.cpp
static std::string currentToken = "";
static std::string cleanToken = "";

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

struct search_pattern {
    token_type tokType;
    char *pattern;
};

search_pattern sPatterns[3];

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
    unsigned int currentLine = 1;

    sPatterns[0].tokType = TOKEN_OP;
    sPatterns[0].pattern = OPS;
    sPatterns[1].tokType = TOKEN_ENDL;
    sPatterns[1].pattern = ";";
    sPatterns[2].tokType = TOKEN_PART;
    sPatterns[2].pattern = "{}(),:";

    // Go through each character one-by-one
    int n = -1;
    char character = raw[0]; 
    while (character != EOF) {
        
        n += 1;
        character = raw[n];
        bool foundToken = false;
        
        // # check for newline characters.
        if (character == '\n') {
            currentLine += 1;
            // NOTE(Noah): We cannot skip here because comments exit on newline.
            // Also, newlines are used to find the end of SYMBOLS.  
        }

        //# if in a comment or multiline comment state, we simply just skip over the current character.
        //# we also wnat to be checking for comment and multi-line comment end states.
        //# TODO(Noah): Implement nested multi-line comments.
        if (state == LEXER_COMMENT) {
            if (character == '\n' || character == EOF) {
                state = LEXER_NORMAL;
                CurrentTokenReset();
            }
            continue;
        } else if (state == LEXER_MULTILINE_COMMENT) {
            if (character == '*' && raw[n+1] == '/') {
                state = LEXER_NORMAL;
                CurrentTokenReset();
                n += 1;
            }
            continue;
        }
        else if (state == LEXER_QUOTE) {
            if (character == '"' && raw[n-1] != '\\') {
                // # end condition to exit quote.
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
            } else { // do to else, \\ is nulled out when used as escape character. i.e. does not appear in final string.
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
                if (QueryForSymbolToken(token, cleanToken, currentLine)) {
                    tokenContainer.Append(token);
                }
                n += 1; //# skip over both '/' characters.
                continue;
            }
            
            //# check for multi-line comments.
            if (character == '/' && raw[n+1] == '*') {
                state = LEXER_MULTILINE_COMMENT;
                Token token;
                if (QueryForSymbolToken(token, cleanToken, currentLine)) {
                    tokenContainer.Append(token);
                }
                n += 1; //# skip over '*', because then we could recognize /*/ as valid.
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
            // Check for decimal literals. 
            if (raw[n] == '.' && IsNumber(cleanToken) ) {
                CurrentTokenAddChar('.');
                continue;
            }

            // Check for any compound operators.
            Token tok;
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
            }

            for (int i = 0; i < 3; i++) {
                struct search_pattern sPattern = sPatterns[i];
                Token tok;
                Token symbolTok;
                QueryForToken(
                    character, 
                    cleanToken, 
                    sPattern.pattern, 
                    sPattern.tokType, 
                    currentLine,
                    tok, 
                    symbolTok
                );
                if (symbolTok.type != TOKEN_UNDEFINED)
                    tokenContainer.Append(symbolTok);
                if (tok.type != TOKEN_UNDEFINED) {
                    tokenContainer.Append(tok);
                    foundToken = true;
                }
            }

            // Check for division statements
            if (character == '/' && raw[n+1] != '/') {
                Token token;
                if (QueryForSymbolToken(token, cleanToken, currentLine)) {
                    tokenContainer.Append(token);
                }
                tokenContainer.Append(Token(TOKEN_OP, "/", currentLine));
                foundToken = true;
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