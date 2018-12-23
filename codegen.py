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

def WriteBinaryInstruction(file, instruction, indentation):
    parent, pair = instruction
    a, b = pair
    if parent == "+":
        WriteStatement(file, "mov eax, " + a + "\n", indentation)
        WriteStatement(file, "mov ebx, " + b + "\n", indentation)
        WriteStatement(file, "add eax, ebx\n", indentation)
        WriteStatement(file, "push eax\n", indentation)
    elif parent == "*":
        WriteStatement(file, "mov eax, " + a + "\n", indentation)
        WriteStatement(file, "mov ebx, " + b + "\n", indentation)
        WriteStatement(file, "add eax, ebx\n", indentation)
        WriteStatement(file, "push eax\n", indentation)
    elif parent == "/":
        pass
    elif parent == "-":
        WriteStatement(file, "mov eax, " + a + "\n", indentation)
        WriteStatement(file, "mov ebx, " + b + "\n", indentation)
        WriteStatement(file, "sub eax, ebx\n", indentation)
        WriteStatement(file, "push eax\n", indentation)

    GLOBAL_INDENTIFIER += 4
    return GLOBAL_INDENTIFIER

def GetASMFromID(id):
    return id

def CopyFromID(file, source, dest, indentation):
    file.write(" " * indentation + "mov " + dest + ", " + GetASMFromID(id) + "\n")

def Run(tree, file):
    for function in tree.children:
        indentation = 0
        WriteStatement(file, "global _start", indentation)
        WriteStatement(file, "_start:", indentation)
        indentation += SPACES_PER_TAB
        for statement in function.children:
            #right now we only support one statement, and it's called return
            #we need to recursivly generate the code for the value that we return

            statementTree = statement.children[0]
            for l in range(-1, -1-statementTree.Len(), -1):
                #2 tree pointers in a tuple, and then a list of those
                pairs = statementTree.GetChildrenPairsAtLayer(l)
                for pair in pairs:
                    parent = pair.GetParent()
                    instruction = (parent, pair)
                    identifier = WriteInstruction(file, instruction, indentation)
                    parent.Set(identifier)
                statementTree.Strip()

            CopyFromID(file, statementTree.data, "eax", indentation)
            WriteStatement(file, "ret", indentation)
