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

def NameMangleSymbol(symbol_obj, fileHandle, logger):
    content = ""
    sym_content =  _Generate_Symbol(symbol_obj, fileHandle, logger)
    parts = sym_content.split("::")
    for part in parts[:-1]:
        content += part + "::"
    content += "_" # NOTE(Noah): Underscore for name mangling purposes.
    content += parts[-1] + '('
    return content

# r"(_symbol)\(((expression)(,(expression))*)?\)"
def _GenerateFunctionCall(ast, fileHandle, logger):
    content = ""
    symbol_obj = ast.children[0]
    
    content += NameMangleSymbol(symbol_obj, fileHandle, logger)
   
    exps = []    
    for child in ast.children[1:]:
        exp_obj = child
        _content, fp = _GenerateExpression(exp_obj, fileHandle, logger)
        exps.append(_content)
    content += ', '.join(exps)
    content += ')'
    return content

def _GenerateLiteral(child, fileHandle, logger):
    content = ""
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
    return content

'''
beta=r"[(function_call)(_symbol)(literal)]",
alpha=r"[(op,++)(op,--)((op,[)(expression)(op,]))((op,.)(object))]",
type="left-recursive"
'''
def _GenerateObject(ast, fileHandle, logger):
    content = ""
    #if len(ast.children) == 1:
    
    # deal with beta first
    # deailing with either a function_call, a _symbol, a literal, or an expression in ().
    child = ast.children[0]
    if "LITERAL" in child.data:
        #.startswith("C_LITERAL:"):
        content += _GenerateLiteral(child, fileHandle, logger)
    elif child.data == "function_call":
        content += _GenerateFunctionCall(child, fileHandle, logger)
    elif child.data == "_symbol":
        content += _Generate_Symbol(child, fileHandle, logger)
    
    if len(ast.children) > 1:
        n = 0
        while n < len(ast.children[1:]):
            child = ast.children[1+n]
            # NOTE(Noah): All possible next nextChildren are ops.
            op = GetOp(child)
            if op == "++" or op == "--":
                # postfix op
                content += op
                n += 1
            elif op == "[":
                # [] subscripting op.
                exp = ast.children[n+2]
                _content, fp = _GenerateExpression(exp, fileHandle, logger)
                content += '[' + _content + ']'
                n += 3
            elif op == ".":
                obj = ast.children[n+2]
                content += '.' + _GenerateObject(obj, fileHandle, logger)
                n += 2

    '''       
    if len(ast.children) == 3:
        # The . operator is going on here
        child = ast.children[0]
        if child.data == "function_call":
            content += _GenerateFunctionCall(child, fileHandle, logger)
        elif child.data == "_symbol":
            content += _Generate_Symbol(child, fileHandle, logger)
        content += '.'
        content += _GenerateObject(ast.children[2], fileHandle, logger)
    elif len(ast.children) == 2:
        # _symbol++ , or maybe --
        content += _Generate_Symbol(ast.children[0], fileHandle, logger)
        content += GetOp(ast.children[1])
    '''
    
    return content

r"[(object)((_sizeof)\([(_symbol)(type)]\))([(op,!)(op,-)(op,&)(op,*)(op,~)(\((type)\))](factor))(\((expression)\))]"
def _GenerateFactor(ast, fileHandle, logger):
    content = ""
    if len(ast.children) == 1:
        # Child is an object or exp
        child = ast.children[0]
        if child.data == "object":
            content += _GenerateObject(ast.children[0], fileHandle, logger)
        elif child.data == "expression":
            _content, fp = _GenerateExpression(child, fileHandle, logger)
            content += '(' + _content + ')'
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
        if child.data == "_sizeof":
            # sizeof operator.
            # NOTE(Noah): Thank god we decided to go with a C backend. Actually getting things done lmao.
            _content = ""
            child2 = ast.children[1]
            if child2.data == "_symbol":
                _content += _Generate_Symbol(child2, fileHandle, logger)
            elif child2.data == "type":
                _content += _GenerateType(child2, fileHandle, logger)
            content += 'sizeof(' + _content + ')'
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
    elif child.data == "logical_or_exp" or child.data == "logical_and_exp" or \
        child.data == "equality_exp" or child.data == "relational_exp" or \
        child.data == "additive_exp" or child.data == "term" or child.data == "bitwise_and_exp" or \
        child.data == "bitwise_or_exp":
        left_obj = child.children[0]
        operator = GetOp(child.children[1])
        right_obj = child.children[2]
        left_content, _pf_flag = _GenerateExpression(left_obj, fileHandle, logger)
        right_content, _pf_flag = _GenerateExpression(right_obj, fileHandle, logger)
        content += left_content + ' ' + operator + ' ' + right_content

        if child.data == "additive_exp" or child.data == "term" or child.data == "logical_or_exp":
            # there may be n many more (OP exp) groups.
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

r"[([((op,[)[(_dynamic)(literal)]?(op,]))(op,->)(_const)](type))(_symbol)(keyword)]"
def _GenerateType(ast, fileHandle, logger):
    content = ""
    if len(ast.children) == 4:
        # Array def, either static or dynamic.
        type_obj = ast.children[3]
        type_content = _GenerateType(type_obj, fileHandle, logger)

        specifier_obj = ast.children[1]
        if "LITERAL:" in specifier_obj.data:
            # dealing with a literal.
            # TODO(Noah): Ensure this is an INTEGER literal and not a decimal literal.
                # this is a semantic checking step.
            if CheckLiteralNumber(specifier_obj):
                # If type of char and arr, special treatment to handle unicode code point sizes.
                l_val = GetLiteralInt(specifier_obj)
                content += type_content
                content += ' [' + str(l_val) + ']'
            else:
                logger.Error("Expected int literal within [] type")
                sys.exit()
        elif specifier_obj.data == "_dynamic":
            # Deailing with a dynamic array.
            # TODO(Noah): Implement custom dynamic arrays via ANSI C backend. 
                # For simplicity we going with C++ std::vector<T> for right now.
            content += "ppl::_Array<{}> ".format(type_content)
        

    elif len(ast.children) == 3:
        # op, [
        # op. ]
        # type
        # we are dealing with an array type, but the elements have not been specified.
        type_obj = ast.children[2]
        type_content = _GenerateType(type_obj, fileHandle, logger)
        content += type_content
        content += ' []'
         

    elif len(ast.children) == 2:
        # There is op and a type, or there is simply the const modifier and a type.
        if ast.children[0].data == "_const":
            type_obj = ast.children[1]
            _type_content = _GenerateType(type_obj, fileHandle, logger)

            # TODO(Noah): Do something more intelligence with const as seen below.
            # dynamic arrays cannot be const.
            # could be some pointers in there.
                # Ex) const ->int != ->const int
                # Ex) const ->int = "pointer to an int that is const" = int * const
                # Ex) -> const int = "pointer to a const int" = const int *
                # Ex) const -> const int = "const pointer to a const int" = const int * const
            # static array, _symbol, or keyword -> const is good on far left.
            content += "const " + _type_content
            
        else:
            type_obj = ast.children[1]
            type_content = _GenerateType(type_obj, fileHandle, logger)
            op_val = GetOp(ast.children[0])
            if op_val == "->":
                # Alas, this is a type which is a defined as a pointer to type!!!
                content += type_content
                content += ' *'
            
            '''# NOTE(Noah): For array types, higher level functions should modify the return values.
            # Need to do (type) (symbol)[]
            # basically the Lv function has some work to do.
            elif op_val == "[]":
                
            '''   


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
            content += _Generate_Symbol(ast.children[0], fileHandle, logger)
    return content

def _GenerateLv(ast, fileHandle, logger):
    content = ""
    type_object = ast.children[0]
    type_content = _GenerateType(type_object, fileHandle, logger)
    symbol = ast.children[1]
    symbol_content = GetSymbol(symbol)

    if "[" in type_content:
        # dealing with an array type, special care needed because C is dumb.
        if "const" in type_content:
            const, type, arr = type_content.split(' ')
            content += const + ' ' + type + ' ' + symbol_content + arr
        else:
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
    def CheckFallthrough(self):
        for s in self.statements:
            child = s.children[0]
            if child.data == "_fallthrough":
                return True
        return False

# TODO(Noah): Right now the default case in the switch statement does not work because 
# we have no way of knowing even if the end statement is simply for another case.....

def GenerateSwitch(ast, fileHandle, logger):
    fileHandle.write("switch(")
    switch_exp = ast.children[0]
    GenerateExpression(switch_exp, fileHandle, logger)
    fileHandle.write(')\n')
    fileHandle.write('{\n')

    # TODO(Noah): Augment generation to account for case: with no EXP.
    # Generate case contexts.
    cases = []
    for child in ast.children[1:]:
        if child.data == "expression":
            cases.append( SwitchContext(child) )
        elif child.data == "statement":
            cases[-1].statements.append(child)
        # NOTE(Noah): note the explicit non-inclusion of the _switch_default option.

    for context in cases:
        no_break = context.CheckFallthrough()
        fileHandle.write("case ")
        GenerateExpression(context.case_exp, fileHandle, logger)
        fileHandle.write(':\n')
        for s in context.statements:
            GenerateStatement(s, fileHandle, logger)
        # PPL lang implicity includes break statement for better switch
        # statements.
        if not no_break:
            fileHandle.write("break;\n")

    # Now we write the default case.
    if ast.children[-1].data == "_switch_default":
        fileHandle.write('default:\n')
        default_switch = ast.children[-1]
        for statement in default_switch.children:
            GenerateStatement(statement, fileHandle, logger)
        fileHandle.write("break;\n")

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
        # NOTE(Noah): Explicit ignoring of the _fallthrough case. This is handled by the parent 
        # switch statement. Fallthrough statement has NO meaning in C backend.

# r"(type)(symbol)\((lv)(,(lv))*\)[;(statement)]"
def GenerateFunction(ast, fileHandle, logger):
    type_object = ast.children[0]
    # NOTE(Noah): Array should never return static array (these are stack stored).
    fileHandle.write(_GenerateType(type_object, fileHandle, logger))
    symbol_text = GetSymbol(ast.children[1])
    if symbol_text == "main":
        fileHandle.write(" main")
    else: 
        fileHandle.write(' _' + symbol_text)
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

r"(lv)(=[(initializer_list)(expression)])?"
def GenerateVarDecl(ast, fileHandle, logger):
    lv_object = ast.children[0]
    GenerateLv(lv_object, fileHandle, logger)
    if len(ast.children) > 1:
        # there exists an expression to set the lv_object to.
        # either an initializer_list or an expression.
        exp = ast.children[1]
        if exp.data == "expression":
            fileHandle.write(' = ')
            GenerateExpression(exp, fileHandle, logger)
        elif exp.data == "initializer_list":
            litMen = []
            for literal in exp.children:
                litty = _GenerateLiteral(literal, fileHandle, logger)
                litMen.append(litty)
            fileHandle.write('= {')
            fileHandle.write(",".join(litMen))
            fileHandle.write('}') 


# r"(keyword=struct)(symbol)\{((var_decl);)*\};"s
def GenerateStructDecl(ast, fileHandle, logger):
    fileHandle.write('struct ')
    symbol_object = ast.children[0]
    fileHandle.write(GetSymbol(symbol_object))
    fileHandle.write('{\n')
    for child in ast.children[1:]:
        fileHandle.write(' ' * TAB_AMOUNT)
        GenerateVarDecl(child, fileHandle, logger)
        fileHandle.write(';\n')
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
