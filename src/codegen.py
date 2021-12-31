# Given an AST, this will generate the corresponding C code that runs it!
# NOTE(Noah): This codegen bit is HIGHLY dependent on the choice of platform...
import grammer as g
import sys
import lexer

TAB_AMOUNT = 4

def CheckLiteralInt(obj):
    if obj.data.startswith("LITERAL:"):
        return lexer.IsNumber(obj.data)
    return False

def GetLiteralString(obj):
    return obj.data[8:]

def GetLiteralInt(obj):
    return int(obj.data[8:])

def GetSymbol(ast):
    return ast.data[7:]

# r"(symbol)\(((expression)(,(expression))*)?\)"
def _GenerateFunctionCall(ast, fileHandle, logger):
    content = ""
    symbol_obj = ast.children[0]
    content += GetSymbol(symbol_obj) + '('
    exps = []    
    for child in ast.children[1:]:
        exp_obj = child
        _content, fp = _GenerateExpression(exp_obj, fileHandle, logger)
        exps.append(_content)
    content += ', '.join(exps)
    content += ')'
    return content

# r"[(literal)(function_call)(symbol)([(op,!)(op,-)](factor))(\((expression)\))]"
def _GenerateFactor(ast, fileHandle, logger):
    content = ""

    child = ast.children[0]
    if child.data.startswith("LITERAL:"):
        content += GetLiteralString(child)
    elif child.data == "function_call":
        content += _GenerateFunctionCall(child, fileHandle, logger)
    elif child.data.startswith("SYMBOL:"):
        # TODO(Noah): This is dumb and stupid.
        content += GetSymbol(child)
    elif child.data == "expression":
        _content, p = _GenerateExpression(child, fileHandle, logger)
        content += _content
    elif child.data.startswith("OP:"): # unary op.
        operator = child.data[3:]
        child2 = ast.children[1]
        content += operator
        _content = _GenerateFactor(child2, fileHandle, logger)
        content += _content
    return content

# r"[(assignment_exp)(conditional_exp)]"
'''"assignment_exp",
        r"(symbol)=(expression)"'''
# "conditional_exp", "logical_or_exp", "logical_and_exp", "equality_exp", "relational_exp", "additive_exp", "term", "factor"
def _GenerateExpression(ast, fileHandle, logger):
    content = ""
    pf_flag = True

    child = ast.children[0]
    if ast.data != "expression":
        child = ast
    
    if child.data == "assignment_exp":
        pf_flag = False # Set pf_flag to true because we are setting the value of some memory.
        symbol_obj = child.children[0]
        exp_obj = child.children[1]
        content += (GetSymbol(symbol_obj) + " = ")
        _content, _pf_flag = _GenerateExpression(exp_obj, fileHandle, logger)
        content += _content
    # r"(logical_or_exp)\?(expression):(expression)"
    elif child.data == "conditional_exp":
        cond_obj = child.children[0]
        true_obj = child.children[1]
        false_obj = child.children[2]
        true_content, _pf_flag = _GenerateExpression(true_obj, fileHandle, logger)
        if CheckLiteralInt(cond_obj) and GetLiteralInt(cond_obj) != 0:
            content += true_content # small prune here.
        else:
            cond_content, _pf_flag = _GenerateExpression(cond_obj, fileHandle, logger)
            content += (cond_content + " ? ")      
            content += (true_content + " : ")
            false_content, _pf_flag = _GenerateExpression(false_obj, fileHandle, logger)
            content += (false_content)
    elif child.data == "logical_or_exp" or child.data == "logicial_and_exp" or \
        child.data == "equality_exp" or child.data == "relational_exp" or \
        child.data == "additive_exp" or child.data == "term":
        left_obj = child.children[0]
        operator = child.children[1].data[3:]
        right_obj = child.children[2]
        left_content, _pf_flag = _GenerateExpression(left_obj, fileHandle, logger)
        right_content, _pf_flag = _GenerateExpression(right_obj, fileHandle, logger)
        content += left_content + ' ' + operator + ' ' + right_content
        if child.data == "additive_exp" or child.data == "term":
            # there may be n many more (+ exp) groups.
            if len(child.children) > 3:
                n = (len(child.children) - 3) // 2
                for _n in range(n):
                    group_op = child.children[3 + _n].data[3:] # TODO(Noah): Make a GetOp function.
                    group_exp = child.children[3 + _n + 1]
                    exp_content, _pf_flag = _GenerateExpression(group_exp, fileHandle, logger)
                    content += ' ' + group_op + exp_content

    elif child.data == "factor":
        # prob check immediately for pf_flag here.
        if child.children[0].data == "function_call":
            pf_flag = False
        factor_content = _GenerateFactor(child, fileHandle, logger)
        content += factor_content

    return (content, pf_flag)

def GenerateExpression(ast, fileHandle, logger):
    content, pf_flag = _GenerateExpression(ast, fileHandle, logger)
    fileHandle.write(content)

def _GenerateType(ast, fileHandle, logger):
    content = ""
    if len(ast.children) == 2:
        # is symbol::symbol
        content += (GetSymbol(ast.children[0]))
        content += ("::")
        content += (GetSymbol(ast.children[1]))
    else:
        # is either symbol or keyword
        data_object_data = ast.children[0].data
        if data_object_data.startswith("KEY:"):
            keyword = data_object_data[4:]
            if g.IsValidType(keyword):
                content += (keyword)
            else:
                # TODO(Noah): Can be much better logging here, like.... what line?
                logger.Error("Invalid core type of {}".format(keyword))
                sys.exit()
        else: # its a symbol
            # SYMBOL:, 7 chars
            content += (GetSymbol(ast.children[0]))
    return content

# r"[((symbol)::(symbol))(symbol)(keyword)]"
def GenerateType(ast, fileHandle, logger):
    fileHandle.write(_GenerateType(ast, fileHandle, logger))

def _GenerateLv(ast, fileHandle, logger):
    content = ""
    type_object = ast.children[0]
    content += _GenerateType(type_object, fileHandle, logger)
    symbol = ast.children[1]
    content += (' ' + GetSymbol(symbol))
    return content

# r"(type)(symbol)"
def GenerateLv(ast, fileHandle, logger):
    fileHandle.write(_GenerateLv(ast, fileHandle, logger))

def GenerateReturn(ast, fileHandle, logger):
    exp_obj = ast.children[0]
    fileHandle.write('return ')
    GenerateExpression(exp_obj, fileHandle, logger)


def GenerateBlock(ast, fileHandle, logger):
    fileHandle.write("{\n")
    fileHandle.IncreaseIndentation(TAB_AMOUNT)
    for statement_obj in ast.children:
        GenerateStatement(statement_obj, fileHandle, logger)
    fileHandle.DecreaseIndentation(TAB_AMOUNT)
    fileHandle.write("}\n")

# r"(keyword=if)\((expression)\)(statement)((keyword=else)(statement))?"
def GenerateIf(ast, fileHandle, logger):
    fileHandle.write("if ")
    fileHandle.write("(")
    exp_obj = ast.children[0]
    GenerateExpression(exp_obj, fileHandle, logger)
    fileHandle.write(")\n")
    statement_obj = ast.children[1]
    GenerateStatement(statement_obj, fileHandle, logger)
    # Check for the else block.
    if len(ast.children) > 2:
        fileHandle.write('else ')
        statement_obj2 = ast.children[2]
        GenerateStatement(statement_obj2, fileHandle, logger)

# r"(keyword=for)\((statement)(expression);(statement_noend)\)(statement)"
def GenerateFor(ast, fileHandle, logger):
    init_statement = ast.children[0]
    eval_exp = ast.children[1]
    end_statement = ast.children[2]
    loop_statement = ast.children[3]
    fileHandle.write("for ")
    fileHandle.write("(")
    GenerateStatement(init_statement, fileHandle, logger)
    GenerateExpression(eval_exp, fileHandle, logger)
    fileHandle.write(';')
    GenerateStatement(end_statement, fileHandle, logger, noend=True)
    fileHandle.write(")\n")
    GenerateStatement(loop_statement, fileHandle, logger)

# r"(keyword=while)\((expression)\)(statement)"
def GenerateWhile(ast, fileHandle, logger):
    eval_exp = ast.children[0]
    loop_statement = ast.children[1]
    fileHandle.write('while(')
    GenerateExpression(eval_exp, fileHandle, logger)
    fileHandle.write(')\n')
    GenerateStatement(loop_statement, fileHandle, logger)

class SwitchContext:
    def __init__(self, case_exp):
        self.case_exp = case_exp
        self.statements = []

# TODO(Noah): Right now the default case in the switch statement does not work because 
# we have no way of knowing even if the end statement is simply for another case.....
# r"(keyword=switch)\((expression)\)\{((keyword=case)(expression):(statement)*)*((keyword=default):(statement)*)?\}"
def GenerateSwitch(ast, fileHandle, logger):
    fileHandle.write("switch(")
    switch_exp = ast.children[0]
    GenerateExpression(switch_exp, fileHandle, logger)
    fileHandle.write(')\n')
    fileHandle.write('{\n')
    # Generate case contexts.
    cases = []
    for child in ast.children[1:]:
        if child.data == "expression":
            cases.append( SwitchContext(child) )
        elif child.data == "statement":
            cases[-1].statements.append(child)
    for context in cases:
        fileHandle.write("case ")
        GenerateExpression(context.case_exp, fileHandle, logger)
        fileHandle.write(':\n')
        for s in context.statements:
            GenerateStatement(s, fileHandle, logger)
    fileHandle.write('}\n')

# r"[;([(var_decl)(expression)(_return)(_break)(_continue)];)(block)(_if)(_for)(_while)(_switch)]"
def GenerateStatement(ast, fileHandle, logger, noend=False):
    children_count = len(ast.children)
    if children_count == 0:
        # Literally just a semi-colon for this statement.
        # So the statement does nothing...
        pass
    else:
        child = ast.children[0]
        if child.data == "var_decl":
            GenerateVarDecl(child, fileHandle, logger)
            if not noend:
                fileHandle.write(';\n')
        elif child.data == "expression":
            content, pure_functional_flag = _GenerateExpression(child, fileHandle, logger)
            if not pure_functional_flag: 
                # Like it actually does something.
                # i.e. a function call or a variable assignemnt.
                # i.e. does it modify memory???
                fileHandle.write(content)
                if not noend:
                    fileHandle.write(';\n')
        elif child.data == "_return":
            GenerateReturn(child, fileHandle, logger)
            fileHandle.write(';\n')
        elif child.data == "_break":
            fileHandle.write("break;\n")
        elif child.data == "_continue":
            fileHandle.write("continue;\n")
        elif child.data == "block":
            GenerateBlock(child, fileHandle, logger)
        elif child.data == "_if":
            GenerateIf(child, fileHandle, logger)
        elif child.data == "_for":
            GenerateFor(child, fileHandle, logger)
        elif child.data == "_while":
            GenerateWhile(child, fileHandle, logger)
        elif child.data == "_switch":
            GenerateSwitch(child, fileHandle, logger)

# r"(type)(symbol)\((lv)(,(lv))*\)[;(statement)]"
def GenerateFunction(ast, fileHandle, logger):
    type_object = ast.children[0]
    GenerateType(type_object, fileHandle, logger)
    symbol_object = ast.children[1]
    fileHandle.write(' ' + GetSymbol(symbol_object))
    fileHandle.write('(')
    if ast.children[-1].data == "statement":
        lvs = [] 
        # function impl
        for child in ast.children[2:-1]:
            lvs.append(_GenerateLv(child, fileHandle, logger))
        fileHandle.write(", ".join(lvs))
        fileHandle.write(')\n')
        GenerateStatement(ast.children[-1], fileHandle, logger)
    else:
        lvs = []
        # function decl
        for child in ast.children[2:-1]:
            lvs.append(_GenerateLv(child, fileHandle, logger))
        fileHandle.write(", ".join(lvs))
        fileHandle.write(');\n')

# r"(lv)(=(expression))?"
def GenerateVarDecl(ast, fileHandle, logger):
    lv_object = ast.children[0]
    GenerateLv(lv_object, fileHandle, logger)
    if len(ast.children) > 1:
        # there exists an expression to set the lv_object to.
        fileHandle.write(' = ')
        exp_obj = ast.children[1]
        GenerateExpression(exp_obj, fileHandle, logger)

# r"(keyword=struct)(symbol)\{((var_decl);)*\};"s
def GenerateStructDecl(ast, fileHandle, logger):
    fileHandle.write('struct ')
    symbol_object = ast.children[0]
    fileHandle.write(GetSymbol(symbol_object))
    fileHandle.write('{')
    for child in ast.children[1:]:
        fileHandle.write(' ' * TAB_AMOUNT)
        GenerateVarDecl(child, fileHandle, logger)
    fileHandle.write('};\n')

# r"[(function)((var_decl);)(struct_decl)]*"
def GenerateProgram(ast, fileHandle, logger):
    for child in ast.children:
        if child.data == "function":
            GenerateFunction(child, fileHandle, logger)
        elif child.data == "var_decl":
            GenerateVarDecl(child, fileHandle, logger)
            fileHandle.write(';\n')
        elif child.data == "struct_decl":
            GenerateStructDecl(child, fileHandle, logger)

class SpecialFilehandle:
    def __init__(self, handle):
        self.handle = handle
        self.indentation = 0
    def IncreaseIndentation(self, amount):
        self.indentation += amount
    def DecreaseIndentation(self, amount):
        self.indentation -= amount
    def write(self, str):
        self.handle.write(str)

def Run(ast, filePath, logger):
    fileHandle = open(filePath, "w")
    sFileHandle = SpecialFilehandle(fileHandle)
    if ast.data == "program":
        GenerateProgram(ast, sFileHandle, logger)
    else:
        logger.Error("AST not a valid program grammer object")
    fileHandle.close()

