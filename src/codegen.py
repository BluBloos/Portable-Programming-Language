# Given an AST, this will generate the corresponding C code that runs it!
# NOTE(Noah): This codegen bit is HIGHLY dependent on the choice of platform...
import grammer as g
import sys
import lexer

TAB_AMOUNT = 4

def CheckLiteralNumber(obj):
    if obj.data.startswith("LITERAL:"):
        return lexer.IsNumber(obj.data[8:])
    return False

def GetLiteralString(obj):
    return obj.data[8:]

def GetLiteralInt(obj):
    return int(obj.data[8:])

def GetOp(ast):
    return ast.data[3:]

def GetSymbol(ast):
    return ast.data[7:]

# r"(_symbol)\(((expression)(,(expression))*)?\)"
def _GenerateFunctionCall(ast, fileHandle, logger):
    content = ""
    symbol_obj = ast.children[0]
    content += _Generate_Symbol(symbol_obj, fileHandle, logger) + '('

    exps = []    
    for child in ast.children[1:]:
        exp_obj = child
        _content, fp = _GenerateExpression(exp_obj, fileHandle, logger)
        exps.append(_content)
    content += ', '.join(exps)
    content += ')'
    return content

r"[([(function_call)(_symbol)](op,.)(object))(function_call)(_symbol)(literal)(\((expression)\))]"
def _GenerateObject(ast, fileHandle, logger):
    content = ""
    if len(ast.children) == 3:
        # The . operator is going on here
        child = ast.children[0]
        if child.data == "function_call":
            content += _GenerateFunctionCall(child, fileHandle, logger)
        elif child.data == "_symbol":
            content += _Generate_Symbol(child, fileHandle, logger)
        content += '.'
        content += _GenerateObject(ast.children[2], fileHandle, logger)

    elif len(ast.children) == 1:
        # deailing with either a function_call, a _symbol, a literal, or an expression in ().
        child = ast.children[0]
        if child.data.startswith("C_LITERAL:"):
            _content = child.data[10:]
            # C_LITERAL
            content += hex(ord(_content))
            #content += _content.encode("utf-8")
            #content += '*(uint64*)"' + _content + '"'
        elif child.data.startswith("LITERAL:"):
            _content = GetLiteralString(child)
            if CheckLiteralNumber(child):
                content += _content
            else:
                # the literal is a QUOTE 
                content += '(string)"' + _content.replace('"', "\\\"") + '"'
        elif child.data == "function_call":
            content += _GenerateFunctionCall(child, fileHandle, logger)
        elif child.data == "_symbol":
            content += _Generate_Symbol(child, fileHandle, logger)
        elif child.data == "expression":
            _content, fp = _GenerateExpression(child, fileHandle, logger)
            content += '(' + _content + ')'
    return content

r"[(object)([(op,!)(op,-)(op,&)(op,*)(\((type)\))](factor))]"
def _GenerateFactor(ast, fileHandle, logger):
    content = ""
    if len(ast.children) == 1:
        # Child is an object
        content += _GenerateObject(ast.children[0], fileHandle, logger)
    elif len(ast.children) == 2:
        # the child is a unary op.
        child = ast.children[0]
        if child.data.startswith("OP:"): # unary op.
            operator = GetOp(child)
            child2 = ast.children[1]
            content += operator
            _content = _GenerateFactor(child2, fileHandle, logger)
            content += _content
        if child.data == "type":
            # Type cast on a factor.
            factor_obj = ast.children[1]
            # NOTE(Noah): Not going to be casting to static array type!
            content += '(' + _GenerateType(child, fileHandle, logger)
            content += ')'
            content += _GenerateFactor(factor_obj, fileHandle, logger)
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
    
    # r"(factor)=(expression)"
    if child.data == "assignment_exp":
        pf_flag = False # Set pf_flag to true because we are setting the value of some memory.
        factor_obj = child.children[0]
        ass_op_obj = child.children[1]
        assignment_op = GetOp(ass_op_obj)
        exp_obj = child.children[2]
        content += _GenerateFactor(factor_obj, fileHandle, logger) + assignment_op
        _content, _pf_flag = _GenerateExpression(exp_obj, fileHandle, logger)
        content += _content
    # r"(logical_or_exp)\?(expression):(expression)"
    elif child.data == "conditional_exp":
        cond_obj = child.children[0]
        true_obj = child.children[1]
        false_obj = child.children[2]
        true_content, _pf_flag = _GenerateExpression(true_obj, fileHandle, logger)
        if CheckLiteralNumber(cond_obj) and GetLiteralInt(cond_obj) != 0:
            content += true_content # small prune here.
        else:
            cond_content, _pf_flag = _GenerateExpression(cond_obj, fileHandle, logger)
            content += (cond_content + " ? ")      
            content += (true_content + " : ")
            false_content, _pf_flag = _GenerateExpression(false_obj, fileHandle, logger)
            content += (false_content)
    elif child.data == "logical_or_exp" or child.data == "logicial_and_exp" or \
        child.data == "equality_exp" or child.data == "relational_exp" or \
        child.data == "additive_exp" or child.data == "term" or child.data == "bitwise_and_exp" or \
        child.data == "bitwise_or_exp":
        left_obj = child.children[0]
        operator = GetOp(child.children[1])
        right_obj = child.children[2]
        left_content, _pf_flag = _GenerateExpression(left_obj, fileHandle, logger)
        right_content, _pf_flag = _GenerateExpression(right_obj, fileHandle, logger)
        content += left_content + ' ' + operator + ' ' + right_content
        if child.data == "additive_exp" or child.data == "term":
            # there may be n many more (+ exp) groups.
            if len(child.children) > 3:
                n = (len(child.children) - 3) // 2
                for _n in range(n):
                    group_op = GetOp(child.children[3 + _n*2])
                    group_exp = child.children[3 + _n*2 + 1]
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

#  r"[((symbol)::(symbol))(symbol)]"
def _Generate_Symbol(ast, fileHandle, logger):
    content = ""
    if len(ast.children) == 2:
       # is symbol::symbol
        content += (GetSymbol(ast.children[0]))
        content += ("::")
        content += (GetSymbol(ast.children[1]))
    else:
        # is a symbol.
        # SYMBOL:, 7 chars
        content += (GetSymbol(ast.children[0]))
    return content

r"[((op,[)(literal)(op,])(type))((op,[])(type))((op,->)(type))(_symbol)(keyword)]"
def _GenerateType(ast, fileHandle, logger):
    content = ""
    if len(ast.children) == 4:
        # static array def with element number specified.
        type_obj = ast.children[3]
        type_content = _GenerateType(type_obj, fileHandle, logger)
        l_obj = ast.children[1]
        # TODO(Noah): Ensure this is an INTEGER literal and not a decimal literal.
        if CheckLiteralNumber(l_obj):
            # If type of char and arr, special treatment to handle unicode code point sizes.
            l_val = GetLiteralInt(l_obj)
            if type_content == "char":
                l_val *= 1 # 32 bits for wide characters to allow utf8-sized strings.
            content += type_content
            content += ' [' + str(l_val) + ']'
        else:
            logger.Error("Expected int literal within [] type")
            sys.exit()
    elif len(ast.children) == 2:
        # There is op and a type, or there is simply the const modifier and a type.
        if ast.children[0].data == "_const":
            # funny thing this is. const is just a compiler directive. There is no underlying behaviour.
            content += _GenerateType(ast.children[1], fileHandle, logger)
            
        else:
            type_obj = ast.children[1]
            type_content = _GenerateType(type_obj, fileHandle, logger)
            op_val = GetOp(ast.children[0])
            if op_val == "->":
                # Alas, this is a type which is a defined as a pointer to type!!!
                content += type_content
                content += ' *'
            # NOTE(Noah): For array types, higher level functions should modify the return values.
            # Need to do (type) (symbol)[]
            # basically the Lv function has some work to do.
            elif op_val == "[]":
                # we are dealing with an array type, but the elements have not been specified.
                content += type_content
                content += ' []'      
    elif len(ast.children) == 1:
        # is either _symbol or keyword
        data_object_data = ast.children[0].data
        if data_object_data.startswith("KEY:"):
            keyword = data_object_data[4:]
            if g.IsValidType(keyword):
                if keyword == "char":
                    content += "uint32"
                else:
                    content += (keyword)
            else:
                # TODO(Noah): Can be much better logging here, like.... what line?
                logger.Error("Invalid core type of {}".format(keyword))
                sys.exit()
        else: # its a _symbol
            content += _Generate_Symbol(ast.childre[0], fileHandle, logger)
    return content

def _GenerateLv(ast, fileHandle, logger):
    content = ""
    type_object = ast.children[0]
    type_content = _GenerateType(type_object, fileHandle, logger)
    symbol = ast.children[1]
    symbol_content = GetSymbol(symbol)

    if "[" in type_content:
        # dealing with an array type, special care needed because C is dumb.
        type, arr = type_content.split(' ')
        content += type + ' ' + symbol_content + arr 
    else:
        content += type_content + ' ' + symbol_content    

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
            fileHandle.write(content)
            if not noend:
                fileHandle.write(';\n')
            if not pure_functional_flag: 
                # NOTE(Noah): We are ignoring the pure_functional_flag. Causing problems. Too hard to check.
                pass
                
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
    # NOTE(Noah): Array should never return static array (these are stack stored).
    fileHandle.write(_GenerateType(type_object, fileHandle, logger))
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

