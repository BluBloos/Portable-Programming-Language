import logger
import sys
import traceback

SPACES_PER_TAB = 2

class ProgramStack:
    def __init__(self):
        self.frames = []

    def Push(self):
        self.frames.append(VariableTable())

    def Pop(self):
        result = self.frames[-1].frameOffset
        self.frames.pop()
        return result

    def Append(self, symbol, size):
        #print("appending " + symbol)
        exist = symbol in self.frames[-1].table
        if not exist:
            self.frames[-1].Append(symbol, size)
        else:
            logger.Error("redefinition of symbol " + symbol)
            for line in traceback.format_stack():
                print(line.strip())
            sys.exit()

    def GetASM(self, symbol):
        #search for the symbol
        container, index = None, -1
        for frame in self.frames[-1::-1]:
            if symbol in frame.table:
                container = frame
                break
            index -= 1

        if container:
            offset = container.table[symbol]
            offset += self.GetOffset(index-1)
            #print("getasm " + symbol + str(container) + " " + str(offset))
            return "[ebp - " + str(offset) + "]"
        logger.Error("symbol " + symbol + " is not defined")
        sys.exit()

    def GetOffset(self, index=-1):
        offset = 0
        for frame in self.frames[index::-1]:
            offset += frame.frameOffset
        return offset

    def Free(self, amount, index=-1):
        self.frames[index].frameOffset -= amount

    def Alloc(self, amount, index=-1):
        self.frames[index].frameOffset += amount

    def print(self):
        index = 0
        for frame in self.frames:
            print("Frame: #" + str(index))
            print(frame.table)
            print("offset: " + str(frame.frameOffset))
            index += 1

class VariableTable:
    def __init__(self):
        self.frameOffset = 0
        self.table = {}

    def Append(self, symbol, size):
        self.frameOffset += size
        self.table[symbol] = self.frameOffset

class LabelMaker:
    def __init__(self):
        self.index = 0
    def MakeLabel(self, symbol):
        self.index += 1
        return symbol + str(self.index)

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
    return "[ebp - " + identifier.replace("id:", "") + "]"

def WriteBinaryInstruction(file, instruction, variableTable, indentation):
    parent, pair = instruction
    a, b = pair
    #a = a.data
    #b = b.data
    parent = parent.data

    if "id:" in a:
        a = GetASMFromID(a)
    elif "var:" in a:
        a = variableTable.GetASM(a.replace("var:", ""))
    if "id:" in b:
        b = GetASMFromID(b)
    elif "var:" in b:
        b = variableTable.GetASM(b.replace("var:", ""))

    if parent == "+":
        WriteStatement(file, "; " + a + " + " + b, indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ebx, " + b, indentation)
        WriteStatement(file, "add eax, ebx", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == "*":
        WriteStatement(file, "; " + a + " * " + b, indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ebx, " + b, indentation)
        WriteStatement(file, "mul ebx", indentation)
        #NOTE(Noah): Here we are truncating the value to 32 bits
        WriteStatement(file, "push eax", indentation)
    elif parent == "/":
        WriteStatement(file, "; " + a + " / " + b, indentation)
        #Clear upper 32 bits of dividend
        WriteStatement(file, "xor edx, edx", indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ebx, " + b, indentation)
        WriteStatement(file, "div ebx", indentation)
        #NOTE(Noah): Here we do a round down because we ignore the remainder
        WriteStatement(file, "push eax", indentation)
    elif parent == "-":
        WriteStatement(file, "; " + a + " - " + b, indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ebx, " + b, indentation)
        WriteStatement(file, "sub eax, ebx", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == "==":
        WriteStatement(file, "; " + a + " == " + b, indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ebx, " + b, indentation)
        WriteStatement(file, "cmp eax, ebx", indentation)
        WriteStatement(file, "mov eax, 0", indentation)
        WriteStatement(file, "sete al", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == "!=":
        WriteStatement(file, "; " + a + " != " + b, indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ebx, " + b, indentation)
        WriteStatement(file, "cmp eax, ebx", indentation)
        WriteStatement(file, "mov eax, 0", indentation)
        WriteStatement(file, "setne al", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == ">":
        WriteStatement(file, "; " + a + " > " + b, indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ebx, " + b, indentation)
        WriteStatement(file, "cmp eax, ebx", indentation)
        WriteStatement(file, "mov eax, 0", indentation)
        WriteStatement(file, "setg al", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == "<":
        WriteStatement(file, "; " + a + " < " + b, indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ebx, " + b, indentation)
        WriteStatement(file, "cmp eax, ebx", indentation)
        WriteStatement(file, "mov eax, 0", indentation)
        WriteStatement(file, "setl al", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == ">=":
        WriteStatement(file, "; " + a + " >= " + b, indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ebx, " + b, indentation)
        WriteStatement(file, "cmp eax, ebx", indentation)
        WriteStatement(file, "mov eax, 0", indentation)
        WriteStatement(file, "setge al", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == "<=":
        WriteStatement(file, "; " + a + " <= " + b, indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ebx, " + b, indentation)
        WriteStatement(file, "cmp eax, ebx", indentation)
        WriteStatement(file, "mov eax, 0", indentation)
        WriteStatement(file, "setle al", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == "||":
        WriteStatement(file, "; " + a + " || " + b, indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ebx, " + b, indentation)
        WriteStatement(file, "or eax, ebx", indentation)
        WriteStatement(file, "mov eax, 0", indentation)
        WriteStatement(file, "setne al", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == "&&":
        WriteStatement(file, "; " + a + " && " + b, indentation)
        #1: register1 becomes a != 0
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "test eax, eax", indentation)
        WriteStatement(file, "mov eax, 0", indentation)
        WriteStatement(file, "setne al", indentation)
        #2: register2 becomes a != 1
        WriteStatement(file, "mov ebx, " + b, indentation)
        WriteStatement(file, "test ebx, ebx", indentation)
        WriteStatement(file, "mov ebx, 0", indentation)
        WriteStatement(file, "setne bl", indentation)
        #3: compute logical and
        WriteStatement(file, "and eax, ebx", indentation)
        WriteStatement(file, "push eax", indentation)

    variableTable.Alloc(4)
    return "id:" + str(variableTable.GetOffset())

def WriteUnaryInstruction(file, instruction, variableTable, indentation):
    parent, child = instruction
    parent = parent.data
    #child = child.data

    if "id:" in child:
        child = GetASMFromID(child)

    if parent == "-":
        WriteStatement(file, "mov eax, " + child, indentation)
        WriteStatement(file, "neg eax", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == "!":
        WriteStatement(file, "mov eax, " + child, indentation)
        WriteStatement(file, "test eax, eax", indentation)
        WriteStatement(file, "mov eax, 0", indentation)
        WriteStatement(file, "sete al", indentation)
        WriteStatement(file, "push eax", indentation)

    variableTable.Alloc(4)
    return "id:" + str(variableTable.GetOffset())

def CopyFromID(file, source, dest, indentation):
    WriteStatement(file, "mov " + dest + ", " + GetASMFromID(source), indentation)

def EvaluateExpressionWrapper(file, expression, variableTable, labelMaker, indentation):
    prevOffset = variableTable.GetOffset()

    result = EvaluateExpression(file, expression, variableTable, labelMaker, indentation)

    #copy the data from the result into eax
    if "id:" in result:
        CopyFromID(file, result, "eax", indentation)
    elif "var:" in result:
        result = variableTable.GetASM(result.replace("var:", ""))
        WriteStatement(file, "mov eax, " + result, indentation)
    else:
        WriteStatement(file, "mov eax, " + result, indentation)

    #deallocate the memory used
    deltaOffset = variableTable.GetOffset() - prevOffset
    if deltaOffset > 0:
        WriteStatement(file, "add esp, " + str(deltaOffset), indentation)
        variableTable.Free(deltaOffset)

def EvaluateExpression(file, expression, variableTable, labelMaker, indentation):
    if expression.children:
        length = len(expression.children)
        if length == 3:
            condition = expression.children[0]
            child1 = expression.children[1]
            child2 = expression.children[2]

            label_term1 = labelMaker.MakeLabel("term1_")
            label_end = labelMaker.MakeLabel("end_")

            WriteStatement(file, "; condition expression:", indentation)
            EvaluateExpression(file, condition, variableTable, labelMaker, indentation)

            WriteStatement(file, "test eax, eax", indentation)
            WriteStatement(file, "; eax != 0:", indentation)
            WriteStatement(file, "mov eax, 0", indentation)
            WriteStatement(file, "setne al", indentation)

            WriteStatement(file, "; eax == 0:", indentation)
            WriteStatement(file, "cmp eax, 1", indentation)
            WriteStatement(file, "je " + label_term1, indentation)

            #put otherwise body here!
            EvaluateExpressionWrapper(file, child2, variableTable, labelMaker, indentation)

            WriteStatement(file, "jmp " + label_end, indentation)
            WriteStatement(file, label_term1 + ":", indentation)

            #put if body here!
            EvaluateExpressionWrapper(file, child1, variableTable, labelMaker, indentation)

            #end
            WriteStatement(file, label_end + ":", indentation)
            WriteStatement(file, "push eax", indentation)
            variableTable.frameOffset += 4
            return "id:" + str(variableTable.frameOffset)

        elif length == 2:
            child1 = expression.children[0]
            child2 = expression.children[1]

            child1 = EvaluateExpression(file, child1, variableTable, labelMaker, indentation)
            child2 = EvaluateExpression(file, child2, variableTable, labelMaker, indentation)

            if IsInteger(child1) and IsInteger(child2):
                op = expression.data
                child1 = int(child1)
                child2 = int(child2)
                if op == "+":
                    return(str(child1 + child2))
                elif op == "-":
                    return(str(child1 - child2))
                elif op == "/":
                    return(str(int(child1 / child2)))
                elif op == "*":
                    return(str(child1 * child2))
                elif op == "==":
                    if child1 == child2:
                        return("1")
                    return("0")
                elif op == "!=":
                     if child1 != child2:
                         return("1")
                     return("0")
                elif op == "&&":
                    if child1 and child2:
                        return("1")
                    return("0")
                elif op == "||":
                     if child1 or child2:
                         return("1")
                     return("0")
                elif op == ">":
                    if child1 > child2:
                        return("1")
                    return("0")
                elif op == "<":
                    if child1 < child2:
                        return("1")
                    return("0")
                elif op == ">=":
                    if child1 >= child2:
                        return("1")
                    return("0")
                elif op == "<=":
                    if child1 <= child2:
                        return("1")
                    return("0")
            else:
                instruction = (expression, (child1, child2))
                return(WriteBinaryInstruction(file, instruction, variableTable, indentation))

        elif length == 1:
            child1 = expression.children[0]
            child1 = EvaluateExpression(file, child1, variableTable, labelMaker, indentation)
            instruction = (expression, (child1))
            return(WriteUnaryInstruction(file, instruction, variableTable, indentation))
    else:
        return expression.data

def EvaluateStatement(file, statement, variableTable, labelMaker, indentation):
    if statement.data == "return":
        WriteStatement(file, "; return:", indentation)
        EvaluateExpressionWrapper(file, statement.children[0], variableTable, labelMaker, indentation)
        #NOTE(Noah): Should we break here, as in not handle any more statements?
        #NOTE(Noah): We can no longer break because we are inside a function!

    elif statement.data == "declaration":
        #NOTE(Noah): This issue with this is that we are adding twice the frameOffset than we need to because
        #evaluate expression actually adds to the frameOffset which is uhhhhhh unfortunate
        WriteStatement(file, "; declaration:", indentation)
        symbol = statement.children[0].data
        variableTable.Append(symbol, 4)
        #print(variableTable.GetOffset())
        #variableTable.print()

        if not len(statement.children) == 2:
            WriteStatement(file, "push dword 0", indentation)
        else:
            expression = statement.children[1]
            prevOffset = variableTable.GetOffset()
            result = EvaluateExpression(file, expression, variableTable, labelMaker, indentation)
            #variableTable.Free(variableTable.GetOffset() - prevOffset)
            #print(variableTable.GetOffset())

            if "var:" in result:
                result = variableTable.GetASM(result.replace("var:", ""))
                WriteStatement(file, "push dword " + result, indentation)
            elif "id:" not in result:
                WriteStatement(file, "push dword " + result, indentation)

    elif statement.data == "assignment":
        symbol = statement.children[0].data
        expression = statement.children[1]
        WriteStatement(file, "; assignment:", indentation)
        var = variableTable.GetASM(symbol)
        EvaluateExpressionWrapper(file, expression, variableTable, labelMaker, indentation)
        WriteStatement(file, "mov " + var + ", eax", indentation)

    elif statement.data == "if":
        expression = statement.children[0]
        body = statement.children[1]

        label_if = labelMaker.MakeLabel("then_")
        label_end = labelMaker.MakeLabel("end_")
        WriteStatement(file, "; if statement:", indentation)
        EvaluateExpressionWrapper(file, expression, variableTable, labelMaker, indentation)
        #eax != 0
        WriteStatement(file, "test eax, eax", indentation)
        WriteStatement(file, "mov eax, 0", indentation)
        WriteStatement(file, "setne al", indentation)
        #eax == 1
        WriteStatement(file, "cmp eax, 1", indentation)
        WriteStatement(file, "je " + label_if, indentation)

        #put otherwise body here!
        if len(statement.children) == 3:
            statement = statement.children[2].children[0]
            EvaluateStatement(file, statement, variableTable, labelMaker, indentation)

        WriteStatement(file, "jmp " + label_end, indentation)
        WriteStatement(file, label_if + ":", indentation)

        #put if body here!
        EvaluateStatement(file, body, variableTable, labelMaker, indentation)
        WriteStatement(file, label_end + ":", indentation)

    elif ":block" in statement.data:
        variableTable.Push()
        for line in statement.children:
            EvaluateStatement(file, line, variableTable, labelMaker, indentation)
        frameOffset = variableTable.Pop()
        WriteStatement(file, "add esp, " + str(frameOffset), indentation)

def Run(tree, file):

    variableTable = ProgramStack()
    labelMaker = LabelMaker()

    for function in tree.children:
        indentation = 0
        functionName = function.data.replace(":block", "")
        WriteStatement(file, "global " + functionName, indentation)
        WriteStatement(file, functionName + ":", indentation)
        indentation += SPACES_PER_TAB
        WriteStatement(file, "push ebp", indentation)
        WriteStatement(file, "mov ebp, esp", indentation)

        EvaluateStatement(file, function, variableTable, labelMaker, indentation)

        WriteStatement(file, "pop ebp", indentation)
        WriteStatement(file, "ret 16", indentation)
