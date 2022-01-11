// NOTE(Noah): This is the full toolchain. The thing that is meant to be run on the command line.

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string>
#include <vector>

typedef unsigned long long uint64;
typedef unsigned int uint32;

enum target_platform {
    MAC_OS,
    WINDOWS,
    POSIX, // TODO(Noah): Should this be UNIX? What is going on here?
    WEB,
    WEB_SCRIPT
};

class Logger {
    public:
    void Error(char *fmt, ...) {
        printf("[ERROR]: ");
        va_list args;
        va_start (args, fmt);
        vprintf(fmt, args);
        va_end (args);
        printf("\n");
    }
    void Log(char *fmt, ...) {
        printf("[LOG]: ");
        va_list args;
        va_start (args, fmt);
        vprintf(fmt, args);
        va_end (args);
        printf("\n");
    }
    // prints minimally.
    void Min(char *fmt, ...) {
        va_list args;
        va_start (args, fmt);
        vprintf(fmt, args);
        va_end (args);
        printf("\n");
    }
};


// Program globals.
Logger LOGGER;

// Compiler parameters.
enum target_platform PLATFORM = POSIX;
bool VERBOSE = false;

enum token_type {

};

struct token {
    enum token_type tokenType;
    union value // 64 bit.
    {
        char *str;
        uint64 u64;
    };
    uint32 line; // TODO(Noah): Add support for programs with more than 4 billion lines.
};

class TokenContainer {
    public:
    std::vector<struct token> tokens;
    void Print() {
        for (int i = 0; i < tokens.size(); i++) {
            struct token tok = tokens[i];
            /*switch(tok.tokenType) {
                 
            }*/
        }
    }
};

class Timer {
    public:
    clock_t begin;
    char *msg;
    Timer(char *msg) : msg(msg) {
        begin = clock();
    }
    ~Timer() {
        clock_t end = clock();
        double cycles = (double)(end - begin);
        double ms = cycles / CLOCKS_PER_SEC * 1000.0f;
        LOGGER.Log("%s took %.2f cyles and %.2f ms", msg, cycles, ms);
    }

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
) {

    // # for the sake of parsing a raw input of ' '
    // # append onto raw.
    // raw += ' '

    RawFileReader raw = RawFileReader(inFile);
    
    //std::string currentToken = ""; 
    enum lexer_state state = LEXER_NORMAL;
    unsigned int currentLine = 1;

    // Go through each character one-by-one
    int n = -1;
    char character = raw[0]; 
    while (character != EOF) {
        
        n += 1;
        character = raw[n];

        // foundToken = False
        
        // # check for newline characters.
        if (character == '\n') {
            currentLine += 1;
            // NOTE(Noah): If we were inside a command, now return to normal state.
            //state = LEXER_NORMAL;
            //continue;  
        }
        else if (character == EOF) {
            // TODO: Dunno what to write here quite yet. Hopefully something awesome!
        }
        else {
            //currentToken += character; //#advance currentToken
            
            // # NOTE(Noah): We cannot skip here because comments exit on newline.
            // # Also, newlines are used to find the end of SYMBOLS.
        }

        // cleanToken = currentToken.strip()

        //# if in a comment or multiline comment state, we simply just skip over the current character.
        //# we also wnat to be checking for comment and multi-line comment end states.
        //# TODO(Noah): Implement nested multi-line comments.
        if (state == LEXER_COMMENT) {
            if (character == '\n' || character == EOF) {
                state = LEXER_NORMAL;
                // currentToken = ""
            }
            continue;
        } else if (state == LEXER_MULTILINE_COMMENT) {
            if (character == '*' && raw[n+1] == '/') {
                state = LEXER_NORMAL;
                // currentToken = ""
                n += 1;
            }
            continue;
        }
        else if (state == LEXER_QUOTE) {
            if (character == '"' && raw[n-1] != '\\') {
                //# end condition to exit quote.
                state = LEXER_NORMAL;
                //tokens.append(Token("QUOTE", currentToken[:-1], currentLine))
                //currentToken = ""
            }
            else if (character == '\\' && raw[n-1] != '\\') {
                //# the escape character is here.
                //# but we must ensure that we pass forward all \n 's.
                if (raw[n+1] != 'n') {
                    //currentToken = currentToken[:-1] # remove escape character.
                }
            }
            continue;
        }
        //# supposing that we are in no comment states, be on the lookout to enter one
        //# TODO(Noah): Make sure that we do not go outside of the range of raw.
        else if (state == LEXER_NORMAL) { 
            
            //# check for single line comments.
            if (character == '/' && raw[n+1] == '/'){
                state = LEXER_COMMENT;
                //token = QueryForSymbolToken(cleanToken, currentLine)
                //if (token):
                //    tokens.append(token)
                n += 1; //# skip over both '/' characters.
                continue;
            }
            
            //# check for multi-line comments.
            if (character == '/' && raw[n+1] == '*') {
                state = LEXER_MULTILINE_COMMENT;
                //token = QueryForSymbolToken(cleanToken, currentLine)
                //if (token):
                //    tokens.append(token)
                n += 1; //# skip over '*', because then we could recognize /*/ as valid.
                continue;
            }

            // Check for beginning of quotes
            if (character == '"') 
            {
                //# Condition for entering the quote state.
                state = LEXER_QUOTE;
                //token = QueryForSymbolToken(cleanToken, currentLine)
                //if (token):
                //    tokens.append(token)
                //currentToken = ""
            }

            // Check for character literals.
            if (raw[n] == '\'' && raw[n+2] == '\'') {
                //# Found a character literal.
                char c_val = raw[n+1];
                //tokens.append(Token("C_LITERAL", c_val, currentLine))
                //currentToken = "" # reset that shit 
                n += 2; //# skip past both character literal and "'"
                continue;
            }

            // Check for decimal literals.
            /* 
            else if raw[n] == "." and IsNumber(cleanToken[:-1]):
                # What we have is a number then a dot. 
                # Simply skip over the . so that it doesn't get caught as an
                # operator. 
                n += 1
                continue
            */

            // Check for compound operator.
            /*result = IsCompoundOp(raw, n, currentLine)
            if result:
                symbolToken = QueryForSymbolToken(cleanToken, currentLine)
                if symbolToken:
                    tokens.append(symbolToken)
                # NOTE(Noah): We must do a continue here because there are some single character
                # ops that are a substring of some compound ops.
                tokens.append(result)
                currentToken = ""
                n += len(result.value)
                continue
            */

            // Check for single character operator.
            /*
            token, symbolToken = QueryForToken(character, cleanToken, OPS, "OP", currentLine)
            if symbolToken:
                tokens.append(symbolToken)
            if token:
                tokens.append(token)
                foundToken = True
            */

            // Check for end lines
            /*
            token, symbolToken = QueryForToken(character, cleanToken, ";", "NULL", currentLine)
            if symbolToken:
                tokens.append(symbolToken)
            if token:
                tokens.append(Token("END", ";", currentLine))
                foundToken = True
            */

            /*
            //# check for seperators
            token, symbolToken = QueryForToken(character, cleanToken, "{}(),:", "PART", currentLine)
            if symbolToken:
                tokens.append(symbolToken)
            if token:
                tokens.append(token)
                foundToken = True
            */

            // #check for division statements
            /*if character == '/' and raw[n+1] != '/':
                token = QueryForSymbolToken(cleanToken, currentLine)
                if (token):
                    tokens.append(token)
                tokens.append(Token("OP", "/", currentLine))
                foundToken = True
            */

            // Check for preprocessor statements
            // TODO(Noah): Process on the spot.
            /*token, symbolToken = QueryForToken(character, cleanToken, "#", "PRE", currentLine)
            if symbolToken:
                tokens.append(symbolToken)
            if token:
                # Need to search until space because prepocessor statements might be #include, #define, etc. 
                length, str = searchUntil(raw, n, " ")
                tokens.append(Token("PRE", str, currentLine))
                currentToken = ""
                n += 1 + length
                continue 
            */

            //#recognize keywords
            //#TODO(Noah): If I were to say int0, I would still recognize the int and 0 seperate
            /*result = IsKeyword(cleanToken, currentLine)
            if result:
                tokens.append(result)
                foundToken = True
            */

            // # NOTE(Noah): Note sure if this should go here...but we will try.
            // # We want to check for symbols if we have hit a space character or newline character (some whitespace).
            /*if (character == ' ' or character == '\n'):
                realLine = currentLine - 1 if character == '\n' else currentLine
                symbolToken = QueryForSymbolToken(cleanToken + ' ', realLine)
                if (symbolToken):
                    tokens.append(symbolToken)
                    foundToken = True
            */

        } 

    } 

    return false;
}

// usage 
// ppl.exe "inFile" -o "outFile" -t "TARGET" [options]
int main(int argc, char **argv)
{
    Timer timer = Timer("ppl.exe");
    LOGGER = Logger();

    const char *userPlatform;
    const char *inFilePath; 
    const char *outFilePath;

    bool paramsValid = false;
    if (argc > 1) {
        inFilePath = argv[1];
        if (argc > 3){
            std::string dash_o = std::string(argv[2]);
            if (dash_o == "-o") {
                outFilePath = argv[3];
                if (argc > 5){
                    std::string dash_t = std::string(argv[4]);
                    if (dash_t == "-t"){
                        userPlatform = argv[5];
                        paramsValid = true;
                    } else
                        LOGGER.Error("-t expected");
                } else
                    LOGGER.Error("No target platform specified");
            } else
                LOGGER.Error("-o expected");
        } else
            LOGGER.Error("No output file specified.");
    } else {
        LOGGER.Error("No input source file specified");
    }

    if (paramsValid) {
        
        FILE *inFile = fopen(inFilePath, "r");
        if (inFile == NULL) {
            LOGGER.Error("inFile of '%s' does not exist", inFilePath);
            return 0; // exit program.
        }
        
        TokenContainer tokensContainer;
        PreparseContext preparseContext;
        if (!LexAndPreparse(inFile, tokensContainer, preparseContext)) {
            // NOTE(Noah): exit program and silently fail. 
            // All error messages are handled by whom that throws err.
            return 0; 
        }
        if (VERBOSE) {
            tokensContainer.Print();
        }

        /*
        raw = file.read()
        file.close()
        tokens = lexer.Run(raw)
        if verbose:
            logger.Log("Printing tokens, pre parser")
            for token in tokens.tokens:
                logger.Log(str(token))
        # compute cwd from inFile.
        cwd = os.path.dirname(inFile)
        pContext = preparser.Run(cwd, tokens, verbose, logger) # Directly modifies the tokens object.
        if verbose:
            logger.Log("Printing pContext")
            logger.Log("libs:"+",".join(pContext.libs))
            logger.Log("targets:"+",".join(pContext.targets))
            logger.Log("Printing tokens, post parser")
            for token in tokens.tokens:
                logger.Log(str(token))
        */

    }
}