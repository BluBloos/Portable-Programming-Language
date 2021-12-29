class Tokens:
    def __init__(self, tokens):
        self.tokens = tokens

    def Len(self):
        return len(self.tokens)    

    def Next(self):
        if len(self.tokens) > 0:
            return self.tokens.pop(0)
        else:
            #TODO(Noah): Fix this inconsistency. Where is the logger?
            print("[ERROR]: No more tokens")
            return False

    def Query(self):
        return self.tokens[0]

    def QueryDistance(self, distance):
        if distance < len(self.tokens):
            return self.tokens[distance]
        else:
            return False

class Token:
    def __init__(self, type, value, line):
        self.type = type
        self.value = value
        self.line = line

def IsKeyword(buffer, line):
    result = -1
    tokenValue = "NULL"
    
    keywords = [
        "struct",
        "true",
        "false",
        "int",
        "void",
        "char",
        "short",
        "unsigned",
        "signed",
        "continue",
        "break",
        "if",
        "while",
        "for",
        "else",
        "return"
    ]

    if buffer in keywords:
        tokenValue = buffer

    if tokenValue != "NULL":
        result = Token("KEY", tokenValue, line)

    if result == -1:
        return False

    return result

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
    if cleanToken[:-1] != "":
        return Token("SYMBOL", cleanToken[:-1].strip(), line)
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

def Run(raw):

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
                currentToken = currentToken[:-1] # remove escape character.
            n += 1
            continue
        elif character == '"': # Condition for entering the quote state.
            state = "QUOTE"
            token = QueryForSymbolToken(cleanToken, currentLine)
            if (token):
                tokens.append(token)
            currentToken = ""

        #handle operators
        token, symbolToken = QueryForToken(character, cleanToken, "+-%*!<>=|&?", "OP", currentLine)
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
            tokens.append(Token("END", "END", currentLine))
            foundToken = True

        #check for seperators
        token, symbolToken = QueryForToken(character, cleanToken, "{}()[].,:", "PART", currentLine)
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