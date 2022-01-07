def CreateNullToken():
    return Token("EOL", "EOL", None)

class Tokens:
    def __init__(self, tokens):
        self.tokens = tokens
        self.savepoint = 0

    def Len(self):
        return len(self.tokens)    

    def GetSavepoint(self):
        return self.savepoint

    def ResetSavepoint(self, newSave):
        self.savepoint = newSave

    def Next(self):
        if self.savepoint < len(self.tokens):
            self.savepoint += 1
            return self.tokens[self.savepoint - 1]
        else:
            #TODO(Noah): Fix this inconsistency. Where is the logger?
            print("[ERROR]: No more tokens")
            return CreateNullToken()

    def QueryNext(self):
        if self.savepoint < len(self.tokens):
            return self.tokens[self.savepoint]
        else:
            return CreateNullToken()

    def QueryDistance(self, distance):
        if distance < len(self.tokens):
            return self.tokens[self.savepoint + distance]
        else:
            return CreateNullToken()

class Token:
    def __init__(self, type, value, line):
        self.type = type
        self.value = value
        self.line = line
    
    def __str__(self):
        return "LN: {}, TYPE: {}, VAL: {}".format(self.line, self.type, self.value)

TYPES = ["float", "double", "int", "char", "short", "string", "bool", "void"]
KEYWORDS = ["struct", "continue", "break", "if", "while", "for", "else", "return", "const", "sizeof"]
OPS = "+-%*!<>=|&?[].~"
COMPOUND_OPS = ['&&', '||', '>=', '<=', '==', '!=', '->', '[]',
    '+=', '-=', '*=', '/=', '%=', '&=', '|=']

def IsCompoundOp(raw, n, line):
    global COMPOUND_OPS
    # NOTE(Noah): Can use a constant of 2 cuz all compounds ops have len of 2
    buffer = raw[n] + raw[n+1]
    if buffer in COMPOUND_OPS:
        return Token("OP", buffer, line)
    return False

def IsKeyword(buffer, line):
    global KEYWORDS
    global TYPES
    result = -1
    tokenValue = "NULL"
    
    # TODO(Noah): Change to a uint8! Or something like
    # this.
    
    if buffer in KEYWORDS or buffer in TYPES:
        tokenValue = buffer

    if tokenValue != "NULL":
        result = Token("KEY", tokenValue, line)

    if result == -1:
        return False

    return result

def IsNumber(potNum):
    if potNum.startswith('.') or potNum.endswith('.'):
        return False
    for char in potNum:
        if char not in "0123456789.":
            return False
    return True

# searches a string buffer str starting at index until string end
# or reaches term. Returns string from index to term (not including)
# plus it returns the length of the string.
# NOTE(Noah): I am realizing now how redudant this is.
def searchUntil(str, index, term):
    sum = 0
    newStr = ""
    for character in str[index:]:
        if character == term:
            break
        newStr += character
        sum += 1
    return (sum, newStr)

def QueryForSymbolToken(cleanToken, line):
    # Supposing that there exists a previous token, which would not include the current character (hence why the strip to [:-1])
    # then this previous token is in fact a symbol.
    # the symbole token does not include current character because symbols are found at the beginning of another token.
    trimmed = cleanToken[:-1]
    if trimmed != "":
        # Check if the symbol is a literal, like a number.
        if IsNumber(trimmed):
            return Token("LITERAL", trimmed.strip(), line)
        elif trimmed == "true":
            return Token("LITERAL", "1", line)
        elif trimmed == "false":
            return Token("LITERAL", "0", line)
        return Token("SYMBOL", trimmed.strip(), line)
    return False

# take current character, the current token but clean.
# also takes a set of test characters.
def QueryForToken(character, cleanToken, test, label, line):
    token = False
    token2 = False
    if character in test:
        token2 = QueryForSymbolToken(cleanToken, line)
        token = Token(label, character, line)
    return (token, token2)

class SafeRaw():
    def __init__(self, raw):
        self.raw = raw
    # NOTE(Noah): We never want to see a negative number in a slice.
    # And this will probaby never happen.
    def __getitem__(self, n):
        if isinstance(n, slice):
            indices = range(*n.indices(len(self.raw)))
            return [self.raw[i] for i in indices]
        if n < 0 or n >= len(self.raw):
            return ' '
        return self.raw[n]
    def __len__(self):
        return len(self.raw)

# TODO(Noah): There are some very unsafe things happeneing here.
# Namely, no checking if out of range of raw...
# NOTE(Noah): Unless the addition of ' ' at the end resolves this.
def Run(raw):
    global OPS

    # for the sake of parsing a raw input of ' '
    # append onto raw.
    raw += ' '

    # construct a special object for safe accessing of char array
    raw = SafeRaw(raw)

    # each of these variables is self-explanatory.
    tokens = []
    currentToken = ""
    state = ""
    currentLine = 1

    # array goes through each character one by one.
    n = 0
    while n < len(raw):
        
        foundToken = False
        character = raw[n] #current character
        # check for newline characters.
        if character != "\n":
            currentToken += character #advance currentToken
        else:
            currentLine += 1
            # NOTE(Noah): We cannot skip here because comments exit on newline.
            # Also, newlines are used to find the end of SYMBOLS.
        cleanToken = currentToken.strip()

        # if in a comment or multiline comment state, we simply just skip over the current character.
        # we also wnat to be checking for comment and multi-line comment end states.
        # TODO(Noah): Implement nested multi-line comments.
        if state == "COMMENT":
            if character == '\n':
                state = ""
                currentToken = ""
            n += 1
            continue
        elif state == "COMMENT_MULTILINE":
            if character == '*' and raw[n+1] == '/':
                state = ""
                currentToken = ""
                n += 1
            n += 1 # we do +=2 to skip over the '/'
            continue
        
        # supposing that we are in no comment states, be on the lookout to enter one
        # TODO(Noah): Make sure that we do not go outside of the range of raw.
        if state != "COMMENT" and state != "COMMENT_MULTILINE": 
            # check for single line comments.
            if character == '/' and raw[n+1] == '/':
                state = "COMMENT"
                token = QueryForSymbolToken(cleanToken, currentLine)
                if (token):
                    tokens.append(token)
                n += 2 # skip over both '/' characters.
                continue
            
            # check for multi-line comments.
            if character == '/' and raw[n+1] == '*':
                state = "COMMENT_MULTILINE"
                token = QueryForSymbolToken(cleanToken, currentLine)
                if (token):
                    tokens.append(token)
                n += 2 # skip over '*', because then we could recognize /*/ as valid.
                continue

        if state == "QUOTE":
            if character == '"' and raw[n-1] != '\\': # end condition to exit quote.
                state = ""
                tokens.append(Token("QUOTE", currentToken[:-1], currentLine))
                currentToken = ""
            if character == '\\' and raw[n-1] != '\\':
                # the escape character is here.
                # but we must ensure that we pass forward all \n 's.
                if raw[n+1] != 'n':
                    currentToken = currentToken[:-1] # remove escape character.
            n += 1
            continue
        elif character == '"': # Condition for entering the quote state.
            state = "QUOTE"
            token = QueryForSymbolToken(cleanToken, currentLine)
            if (token):
                tokens.append(token)
            currentToken = ""
        
        if state not in ["QUOTE", "COMMENT", "COMMENT_MULTILINE"]:
            if raw[n] == "'" and raw[n+2] == "'":
                # Found a character literal.
                c_val = raw[n+1]
                tokens.append(Token("C_LITERAL", c_val, currentLine))
                currentToken = "" # reset that shit 
                n += 3 # skip past both character literal and "'"
                continue
            elif raw[n] == "." and IsNumber(cleanToken[:-1]):
                # What we have is a number then a dot. 
                # Simply skip over the . so that it doesn't get caught as an
                # operator. 
                n += 1
                continue
        
        #check for a compound operator.
        result = IsCompoundOp(raw, n, currentLine)
        if result:
            # NOTE(Noah): We must do a continue here because there are some single character
            # ops that are a substring of some compound ops.
            tokens.append(result)
            currentToken = ""
            n += len(result.value)
            continue 

        #handle operators
        token, symbolToken = QueryForToken(character, cleanToken, OPS, "OP", currentLine)
        if symbolToken:
            tokens.append(symbolToken)
        if token:
            tokens.append(token)
            foundToken = True

        #check for end lines
        token, symbolToken = QueryForToken(character, cleanToken, ";", "NULL", currentLine)
        if symbolToken:
            tokens.append(symbolToken)
        if token:
            tokens.append(Token("END", ";", currentLine))
            foundToken = True

        #check for seperators
        # NOTE(Noah): We have to add back the . somewhere in the lexer. Right now it gets
        # consumed by literals...
        token, symbolToken = QueryForToken(character, cleanToken, "{}(),:", "PART", currentLine)
        if symbolToken:
            tokens.append(symbolToken)
        if token:
            tokens.append(token)
            foundToken = True

        #check for division statements
        if character == '/' and raw[n+1] != '/':
            token = QueryForSymbolToken(cleanToken, currentLine)
            if (token):
                tokens.append(token)
            tokens.append(Token("OP", "/", currentLine))
            foundToken = True

        #check for preprocessor statements
        token, symbolToken = QueryForToken(character, cleanToken, "#", "PRE", currentLine)
        if symbolToken:
            tokens.append(symbolToken)
        if token:
             # Need to search until space because prepocessor statements might be #include, #define, etc. 
             length, str = searchUntil(raw, n, " ")
             tokens.append(Token("PRE", str, currentLine))
             currentToken = ""
             n += 1 + length
             continue

        #recognize keywords
        #TODO(Noah): If I were to say int0, I would still recognize the int and 0 seperate
        result = IsKeyword(cleanToken, currentLine)
        if result:
            tokens.append(result)
            foundToken = True

        # NOTE(Noah): Note sure if this should go here...but we will try.
        # We want to check for symbols if we have hit a space character or newline character (some whitespace).
        if (character == ' ' or character == '\n'):
            realLine = currentLine - 1 if character == '\n' else currentLine
            symbolToken = QueryForSymbolToken(cleanToken + ' ', realLine)
            if (symbolToken):
                tokens.append(symbolToken)
                foundToken = True

        if foundToken:
            currentToken = ""

        # After each 'for loop' iteration, increment n
        n += 1 

    return Tokens(tokens)