SPACES_PER_TAB = 2
GLOBAL_INDENTIFIER = 0

def WriteStatement(file, statement, indentation):
    file.write(" " * indentation + statement + "\n")

def IsInteger(string):
    for char in string:
        if char not in "0123456789":
            return False
    return True

def IsUnary(char):
    if char in "!-":
        return True
    return False

def GetASMFromID(identifier):
    return "[ebp - " + identifier + "]"

def WriteBinaryInstruction(file, instruction, indentation):
    parent, pair = instruction
    a, b = pair
    a = a.data
    b = b.data
    parent = parent.data
    global GLOBAL_INDENTIFIER

    if "id:" in a:
        a = GetASMFromID(a)
    if "id:" in b:
        b = GetASMFromID(b)

    if parent == "+":
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ebx, " + b, indentation)
        WriteStatement(file, "add eax, ebx", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == "*":
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ebx, " + b, indentation)
        WriteStatement(file, "mul ebx", indentation)
        #NOTE(Noah): Here we are truncating the value to 32 bits
        WriteStatement(file, "push eax", indentation)
    elif parent == "/":
        #Clear upper 32 bits of dividend
        WriteStatement(file, "xor edx, edx", indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ebx, " + b, indentation)
        WriteStatement(file, "div ebx", indentation)
        #NOTE(Noah): Here we do a round down because we ignore the remainder
        WriteStatement(file, "push eax", indentation)
    elif parent == "-":
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ebx, " + b, indentation)
        WriteStatement(file, "sub eax, ebx", indentation)
        WriteStatement(file, "push eax", indentation)

    GLOBAL_INDENTIFIER += 4
    return "id:" + str(GLOBAL_INDENTIFIER)

def WriteUnaryInstruction(file, instruction, indentation):
    parent, child = instruction
    parent = parent.data
    child = child.data

    if "id:" in child:
        child = GetASMFromID(child)

    if parent == "-":
        WriteStatement(file, "mov eax, " + child, indentation)
        WriteStatement(file, "neg eax", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == "!":
        WriteStatement(file, "mov eax, " + child, indentation)
        WriteStatement(file, "test eax, eax", indentation)
        WriteStatement(file, "xor eax, eax", indentation)
        WriteStatement(file, "sete eax", indentation)
        WriteStatement(file, "push eax", indentation)

    GLOBAL_INDENTIFIER += 4
    return "id:" + str(GLOBAL_INDENTIFIER)

def CopyFromID(file, source, dest, indentation):
    WriteStatement(file, "mov " + dest + ", " + GetASMFromID(source), indentation)

def Run(tree, file):
    for function in tree.children:
        indentation = 0
        WriteStatement(file, "global " + function.data, indentation)
        WriteStatement(file, function.data + ":", indentation)
        indentation += SPACES_PER_TAB
        for statement in function.children:
            #right now we only support the return statement
            #we need to recursivly generate the code for the value that we return

            WriteStatement(file, "push ebp", indentation)
            WriteStatement(file, "mov ebp, esp", indentation)

            statement = statement.children[0]
            for l in range(statement.depth-1, 0,-1):
                print("l: " + str(l))
                #2 tree pointers in a tuple, and then a list of those
                families = statement.GetFamilies(l)
                print(families)
                for parent, pair in families:
                    print("PARENT:")
                    parent.Print(0)
                    print("CHILD1:")
                    a, b = pair
                    a.Print(0)
                    #instruction = (parent, pair)
                    if b:
                        print("CHILD2:")
                        b.Print(0)
                        instruction = (parent, pair)
                        identifier = WriteBinaryInstruction(file, instruction, indentation)
                        parent.data = identifier
                    else:
                        instruction = (parent, a)
                        identifier = WriteUnaryInstruction(file, instruction, indentation)
                        parent.data = identifier
                statement.Strip()

            CopyFromID(file, statement.data, "eax", indentation)
            WriteStatement(file, "add esp, " + str(GLOBAL_INDENTIFIER), indentation)
            WriteStatement(file, "pop ebp", indentation)
            WriteStatement(file, "ret", indentation)
