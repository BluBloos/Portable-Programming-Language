class Tokens:
    def __init__(self, tokens):
        self.tokens = tokens
    def Next(self):
        return self.tokens.pop(0)
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
    result = ()
    tokenValue = "NULL"
    if buffer == "struct":
        tokenValue = buffer
    elif buffer == "true":
        tokenValue = "true"
    elif buffer == "false":
        tokenValue = "false"
    elif buffer == "int":
        tokenValue = buffer
    elif buffer == "unsigned":
        tokenValue = buffer
    elif buffer == "char":
        tokenValue = buffer
    elif buffer == "short":
        tokenValue = buffer
    elif buffer == "if":
        tokenValue = buffer
    elif buffer == "while":
        tokenValue = buffer
    elif buffer == "for":
        tokenValue = buffer
    elif buffer == "else":
        tokenValue = buffer
    elif buffer == "return":
        tokenValue = buffer

    if tokenValue != "NULL":
        result = Token("KEY", tokenValue, line)

    if result == ():
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

def QueryForToken(character, cleanToken, test, label, line):
    token = False
    token2 = False
    if character in test:

        if cleanToken[:-1] != "":
            token2 = Token("SYMBOL", cleanToken[:-1].strip(), line)
        token = Token(label, character, line)

    return (token, token2)

def Run(raw):

    tokens = []
    currentToken = ""
    state = ""
    currentLine = 1

    for n in range(len(raw)):

        foundToken = False
        character = raw[n] #current character

        if character != "\n":
            currentToken += character #advance currentToken
        else:
            currentLine += 1

        cT = currentToken.strip()

        if state == "COMMENT":
            if character == '\n':
                state = ""
                currentToken = ""
            continue
        elif character == '/' and raw[n+1] == '/':
            state = "COMMENT"
            if cT[:-1] != "":
                tokens.append(Token("SYMBOL", cT[:-1].strip(), currentLine))

        if state == "QUOTE":
            if character == '"':
                state = ""
                tokens.append(Token("QUOTE", currentToken[:-1], currentLine))
                currentToken = ""
            continue
        elif character == '"':
            state = "QUOTE"
            if cT[:-1] != "":
                tokens.append(Token("SYMBOL", cT[:-1].strip(), currentLine))
            currentToken = ""

        if state == "WAIT_SPACE":
            if character == " ":
                state = ""
                currentToken = ""
            continue

        #handle operators
        token, token2 = QueryForToken(character, cT, "+-%*!<>=|&?", "OP", currentLine)
        if token2:
            tokens.append(token2)
        if token:
            tokens.append(token)
            foundToken = True

        #check for end lines
        token, token2 = QueryForToken(character, cT, ";", "NULL", currentLine)
        if token2:
            tokens.append(token2)
        if token:
            tokens.append(Token("END", "END", currentLine))
            foundToken = True

        #check for seperators
        token, token2 = QueryForToken(character, cT, "{}()[].,:", "PART", currentLine)
        if token2:
            tokens.append(token2)
        if token:
            tokens.append(token)
            foundToken = True

        #check for division statements
        if character == '/' and raw[n+1] != '/':
            if cT[:-1] != "":
                tokens.append(Token("SYMBOL", cT[:-1].strip(), currentLine))
            tokens.append(Token("OP", "/", currentLine))
            foundToken = True

        #check for preprocessor statements
        token, token2 = QueryForToken(character, cT, "#", "PRE", currentLine)
        if token2:
            tokens.append(token2)
        if token:
             length, str = searchUntil(raw, n, " ")
             tokens.append(Token("PRE", str, currentLine))
             state = "WAIT_SPACE"
             foundToken = True

        #recognize keywords
        #TODO(Noah): If I were to say int0, I would still recognize the int and 0 seperate
        result = IsKeyword(cT, currentLine)
        if result:
            tokens.append(result)
            foundToken = True

        if foundToken:
            currentToken = ""

    return Tokens(tokens)
