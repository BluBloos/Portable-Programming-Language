import logger
import sys
import traceback

SPACES_PER_TAB = 2
PLATFORM = "WINDOWS"

class ProgramStack:
    def __init__(self):
        self.frames = []

    def Push(self):
        self.frames.append(VariableTable())

    def Pop(self):

        # NOTE: This function should never be called
        # when the stack is empty
        if (len(self.frames) == 0):
            logger.Error("Trying to pop empty program stack")
            return 0

        result = self.frames[-1].frameOffset
        self.frames.pop()
        return result

    # Adopt is for adding variables to a frame without increasing the offset.
    # Good for taking variables from another frame (happens in scoping).
    def Adopt(self, symbol, offset, direction):
        exist = symbol in self.frames[-1].table
        if not exist:
            self.frames[-1].table[symbol] = (offset, direction)
        else:
            logger.Error("redefinition of symbol " + symbol)
            #for line in traceback.format_stack():
            #    print(line.strip())
            sys.exit()

    def Append(self, symbol, size):
        #print("appending " + symbol)
        exist = symbol in self.frames[-1].table
        if not exist:
            self.frames[-1].Append(symbol, size)
        else:
            logger.Error("redefinition of symbol " + symbol)
            #for line in traceback.format_stack():
            #    print(line.strip())
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
            offset, direction = container.table[symbol]
            offset += self.GetOffset(index-1)
            if direction == "forward":
                return "[ebp - " + str(offset) + "]"
            elif direction == "backward":
                return "[ebp + " + str(offset) + "]"

        logger.Error("symbol " + symbol + " is not defined")
        sys.exit()

    def GetOffset(self, index=-1):
        offset = 0
        for frame in self.frames[index::-1]:
            offset += frame.frameOffset
        return offset

    # NOTE: Free and Alloc are used when expressions are pushed to the Stack
    # and not stored in any explicit variable.

    def Free(self, amount, index=-1):
        self.frames[index].frameOffset -= amount

    def Alloc(self, amount, index=-1):
        self.frames[index].frameOffset += amount

    def __str__(self):
        str_frames = [str(x) for x in self.frames]
        result = ", ".join(str_frames)
        return result


class VariableTable:
    def __init__(self):
        self.frameOffset = 0
        self.table = {}

    def Append(self, symbol, size):
        self.frameOffset += size
        self.table[symbol] = (self.frameOffset, "forward")

    def __str__(self):
        result = "[frameOffset: {}, table: {}]".format(self.frameOffset, self.table)
        return result

class LabelMaker:
    def __init__(self):
        self.index = 0
    def MakeLabel(self, symbol):
        self.index += 1
        return symbol + str(self.index)

def PlatformReturnStatement(functionName):
    if PLATFORM == "WINDOWS" and functionName == "main":
        return "ret 16"
    else:
        return "ret"

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
        WriteStatement(file, "mov ecx, " + b, indentation)
        WriteStatement(file, "add eax, ecx", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == "*":
        WriteStatement(file, "; " + a + " * " + b, indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ecx, " + b, indentation)
        WriteStatement(file, "mul ecx", indentation)
        #NOTE(Noah): Here we are truncating the value to 32 bits
        WriteStatement(file, "push eax", indentation)
    elif parent == "/":
        WriteStatement(file, "; " + a + " / " + b, indentation)
        #Clear upper 32 bits of dividend
        WriteStatement(file, "xor edx, edx", indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ecx, " + b, indentation)
        WriteStatement(file, "div ecx", indentation)
        #NOTE(Noah): Here we do a round down because we ignore the remainder
        WriteStatement(file, "push eax", indentation)
    elif parent == "-":
        WriteStatement(file, "; " + a + " - " + b, indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ecx, " + b, indentation)
        WriteStatement(file, "sub eax, ecx", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == "==":
        WriteStatement(file, "; " + a + " == " + b, indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ecx, " + b, indentation)
        WriteStatement(file, "cmp eax, ecx", indentation)
        WriteStatement(file, "mov eax, 0", indentation)
        WriteStatement(file, "sete al", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == "!=":
        WriteStatement(file, "; " + a + " != " + b, indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ecx, " + b, indentation)
        WriteStatement(file, "cmp eax, ecx", indentation)
        WriteStatement(file, "mov eax, 0", indentation)
        WriteStatement(file, "setne al", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == ">":
        WriteStatement(file, "; " + a + " > " + b, indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ecx, " + b, indentation)
        WriteStatement(file, "cmp eax, ecx", indentation)
        WriteStatement(file, "mov eax, 0", indentation)
        WriteStatement(file, "setg al", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == "<":
        WriteStatement(file, "; " + a + " < " + b, indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ecx, " + b, indentation)
        WriteStatement(file, "cmp eax, ecx", indentation)
        WriteStatement(file, "mov eax, 0", indentation)
        WriteStatement(file, "setl al", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == ">=":
        WriteStatement(file, "; " + a + " >= " + b, indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ecx, " + b, indentation)
        WriteStatement(file, "cmp eax, ecx", indentation)
        WriteStatement(file, "mov eax, 0", indentation)
        WriteStatement(file, "setge al", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == "<=":
        WriteStatement(file, "; " + a + " <= " + b, indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ecx, " + b, indentation)
        WriteStatement(file, "cmp eax, ecx", indentation)
        WriteStatement(file, "mov eax, 0", indentation)
        WriteStatement(file, "setle al", indentation)
        WriteStatement(file, "push eax", indentation)
    elif parent == "||":
        WriteStatement(file, "; " + a + " || " + b, indentation)
        WriteStatement(file, "mov eax, " + a, indentation)
        WriteStatement(file, "mov ecx, " + b, indentation)
        WriteStatement(file, "or eax, ecx", indentation)
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
        WriteStatement(file, "mov ecx, " + b, indentation)
        WriteStatement(file, "test ecx, ecx", indentation)
        WriteStatement(file, "mov ecx, 0", indentation)
        WriteStatement(file, "setne bl", indentation)
        #3: compute logical and
        WriteStatement(file, "and eax, ecx", indentation)
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
        if "call:" in expression.data:
            symbol = expression.data.replace("call:", "")
            #we are going to call the function! And then push eax to generate ID
            #Push the children on the stack from right to left!

            prevOffset = variableTable.GetOffset()
            totalParamSize = 0
            for child in expression.children[-1::-1]:
                #print("evalutating childs")
                result = EvaluateExpression(file, child, variableTable, labelMaker, indentation)
                if "var:" in result:
                    result = variableTable.GetASM(result.replace("var:", ""))
                    WriteStatement(file, "push dword " + result, indentation)
                elif IsInteger(result):
                    WriteStatement(file, "push dword " + result, indentation)
                totalParamSize += 4

            WriteStatement(file, "call " + symbol, indentation)

            # free the stack space used to generate then function params,
            # and free the variable table offset generate by intermediate values
            # for the function param
            WriteStatement(file, "add esp, " + str(totalParamSize), indentation)
            deltaOffset = variableTable.GetOffset() - prevOffset
            variableTable.Free(deltaOffset)

            WriteStatement(file, "push eax", indentation)

            print(variableTable)
            variableTable.Alloc(4)
            return "id:" + str(variableTable.GetOffset())

        else:
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

                variableTable.Alloc(4)
                return "id:" + str(variableTable.GetOffset())

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

def EvaluateStatement(functionName, file, statement, variableTable, labelMaker, indentation, args=[]):
    if statement.data == "return":
        WriteStatement(file, "; return:", indentation)
        EvaluateExpressionWrapper(file, statement.children[0], variableTable, labelMaker, indentation)

        #frameOffset = variableTable.Pop()
        #if frameOffset > 0:

        # NOTE: The total frame offset should be the scope of the containing function
        # for the return statement. As of writing this comment, the compiler does
        # not support defining functions in functions. Meaning if we use the offset of
        # the variable table, which is actually program stack, then .GetOffset() returns
        # the value we want.
        totalFrameOffset = variableTable.GetOffset()
        if totalFrameOffset > 0:
            WriteStatement(file, "add esp, " + str(totalFrameOffset), indentation)

        WriteStatement(file, "pop ebp", indentation)
        WriteStatement(file, PlatformReturnStatement(functionName), indentation)
        return True
        #NOTE(Noah): Should we break here, as in not handle any more statements?
        #NOTE(Noah): We can no longer break because we are inside a function!

    elif statement.data == "declaration":
        #NOTE(Noah): This issue with this is that we are adding twice the frameOffset than we need to because
        #evaluate expression actually adds to the frameOffset which is uhhhhhh unfortunate
        WriteStatement(file, "; declaration:", indentation)
        symbol = statement.children[0].data
        pushed = False

        if not len(statement.children) == 2:
            WriteStatement(file, "push dword 0", indentation)
            pushed = True
        else:
            expression = statement.children[1]
            prevOffset = variableTable.GetOffset()
            result = EvaluateExpression(file, expression, variableTable, labelMaker, indentation)

            if "var:" in result:
                result = variableTable.GetASM(result.replace("var:", ""))
                WriteStatement(file, "push dword " + result, indentation)
                pushed = True
            elif "id:" not in result:
                WriteStatement(file, "push dword " + result, indentation)
                pushed = True

        if pushed:
            variableTable.Append(symbol, 4)
        else:
            #NOTE(Noah): This is such a bodge. I'm abusing the fact that I can have diff sized variables to generate a symbol with size 0, because the frameOffset
            #has already been nudged. I have to do this so that the variable is "well defined".
            variableTable.Append(symbol, 0)


    elif statement.data == "assignment":
        symbol = statement.children[0].data
        expression = statement.children[1]
        WriteStatement(file, "; assignment:", indentation)
        var = variableTable.GetASM(symbol)
        EvaluateExpressionWrapper(file, expression, variableTable, labelMaker, indentation)
        WriteStatement(file, "mov " + var + ", eax", indentation)

    elif statement.data == "if":
        done = False
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
            if EvaluateStatement(functionName, file, statement, variableTable, labelMaker, indentation, args):
                done = True

        WriteStatement(file, "jmp " + label_end, indentation)
        WriteStatement(file, label_if + ":", indentation)

        #put if body here!
        if EvaluateStatement(functionName, file, body, variableTable, labelMaker, indentation, args):
            done = True

        WriteStatement(file, label_end + ":", indentation)
        return done

    elif statement.data == "for":
        done = False
        WriteStatement(file, "; for loop:", indentation)
        #NOTE(Noah): the header of a for loop is actually in it's own scope. Isn't that crazy?
        variableTable.Push()

        init = statement.children[0]
        condition = statement.children[1]
        end = statement.children[2]
        body = statement.children[3]

        continueLabel = labelMaker.MakeLabel("continue")
        breakLabel = labelMaker.MakeLabel("break")

        if EvaluateStatement(functionName, file, init, variableTable, labelMaker, indentation, args):
            done = True

        WriteStatement(file, continueLabel + ":", indentation)

        #condition code here
        EvaluateExpressionWrapper(file, condition, variableTable, labelMaker, indentation)
        #eax != 0
        WriteStatement(file, "test eax, eax", indentation)
        WriteStatement(file, "mov eax, 0", indentation)
        WriteStatement(file, "setne al", indentation)
        #eax == 1
        WriteStatement(file, "cmp eax, 1", indentation)
        WriteStatement(file, "jne " + breakLabel, indentation)

        if EvaluateStatement(functionName, file, body, variableTable, labelMaker, indentation, [breakLabel, continueLabel]):
            done = True

        if EvaluateStatement(functionName, file, end, variableTable, labelMaker, indentation, args):
            done = True

        WriteStatement(file, "jmp " + continueLabel, indentation)
        WriteStatement(file, breakLabel + ":", indentation)

        frameOffset = variableTable.Pop()
        if frameOffset > 0:
            WriteStatement(file, "add esp, " + str(frameOffset), indentation)
        return done

    elif statement.data == "break":
        print(args)
        breakLabel = args[0]
        WriteStatement(file, "jmp " + breakLabel, indentation)

    elif statement.data == "continue":
        continueLabel = args[1]
        WriteStatement(file, "jmp " + continueLabel, indentation)

    elif statement.data == "while":
        WriteStatement(file, "; while loop:", indentation)
        condition = statement.children[0]
        body = statement.children[1]

        continueLabel = labelMaker.MakeLabel("continue")
        breakLabel = labelMaker.MakeLabel("break")

        WriteStatement(file, continueLabel + ":", indentation)
        #condition code here
        EvaluateExpressionWrapper(file, condition, variableTable, labelMaker, indentation)
        #eax != 0
        WriteStatement(file, "test eax, eax", indentation)
        WriteStatement(file, "mov eax, 0", indentation)
        WriteStatement(file, "setne al", indentation)
        #eax == 1
        WriteStatement(file, "cmp eax, 1", indentation)
        WriteStatement(file, "jne " + breakLabel, indentation)

        EvaluateStatement(functionName, file, body, variableTable, labelMaker, indentation, [breakLabel, continueLabel])

        WriteStatement(file, "jmp " + continueLabel, indentation)
        WriteStatement(file, breakLabel + ":", indentation)


    elif ":block" in statement.data:
        done = False
        variableTable.Push()
        for line in statement.children:
            if EvaluateStatement(functionName, file, line, variableTable, labelMaker, indentation, args):
                done = True

        frameOffset = variableTable.Pop()
        if frameOffset > 0:
            WriteStatement(file, "add esp, " + str(frameOffset), indentation)

        return done

def Run(platform, tree, file):

    PLATFORM = platform
    variableTable = ProgramStack()
    labelMaker = LabelMaker()

    # For function section -> assembly text section
    WriteStatement(file, "section .text", 0)

    # iterate through each function node
    for function in tree.children:
        indentation = 0
        functionName = function.data
        variableTable.Push()

        body = None
        offset = 4 # account for stack frame pointer on the stack
        for child in function.children:
            if ":block" in child.data:
                body = child
            else:
                # if the child is not the function body, it is a function parameter
                offset += 4
                variableTable.Adopt(child.data, offset, "backward")


        if (functionName == "main"):
            WriteStatement(file, "global start", indentation)
            WriteStatement(file, "start:", indentation)
        else:
            WriteStatement(file, functionName + ":", indentation)
        indentation += SPACES_PER_TAB
        #Setup the function stack frame
        WriteStatement(file, "push ebp", indentation)
        WriteStatement(file, "mov ebp, esp", indentation)

        if body:
            good = EvaluateStatement(functionName, file, body, variableTable, labelMaker, indentation)

        #print("Program Stack Offset at end: {}".format(variableTable.GetOffset()))
        #print("Program Stack: {}".format(variableTable))

        #WriteStatement(file, "ret 16", indentation)
        frameOffset = variableTable.Pop()

        #if frameOffset > 0:
        WriteStatement(file, "add esp, " + str(frameOffset), indentation)

        # Default return value of functions is zero
        WriteStatement(file, "pop ebp", indentation)
        WriteStatement(file, "xor eax, eax", indentation)
        WriteStatement(file, PlatformReturnStatement(functionName), indentation)
