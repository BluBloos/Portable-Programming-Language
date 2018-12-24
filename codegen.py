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
    parent = parent.Get()
    a = a.Get()
    b = b.Get()

    if parent == "+":
        WriteStatement(file, "mov eax, " + a + "\n", indentation)
        WriteStatement(file, "mov ebx, " + b + "\n", indentation)
        WriteStatement(file, "add eax, ebx\n", indentation)
        WriteStatement(file, "push eax\n", indentation)
    elif parent == "*":
        WriteStatement(file, "mov eax, " + a + "\n", indentation)
        WriteStatement(file, "mov ebx, " + b + "\n", indentation)
        WriteStatement(file, "mul ebx\n", indentation)
        #NOTE(Noah): Here we are truncating the value to 32 bits
        WriteStatement(file, "push eax\n", indentation)
    elif parent == "/":
        #Clear upper 32 bits of dividend
        WriteStatement(file, "xor edx, edx\n", indentation)
        WriteStatement(file, "mov eax, " + a + "\n", indentation)
        WriteStatement(file, "mov ebx, " + b + "\n", indentation)
        WriteStatement(file, "div ebx\n", indentation)
        #NOTE(Noah): Here we do a round down because we ignore the remainder
        WriteStatement(file, "push eax\n", indentation)
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
                parents, pairs = statementTree.GetFamilies(l)
                for instruction in zip(parents, pairs):
                    parent, pair = instruction
                    identifier = WriteBinaryInstruction(file, instruction, indentation)
                    parent.Set(identifier)
                statementTree.Strip()

            CopyFromID(file, statementTree.data, "eax", indentation)
            WriteStatement(file, "ret", indentation)
