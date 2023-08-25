#ifndef CODEGEN_H
#define CODEGEN_H

// NOTE: CG = codegen

struct CG_String
{
    char *backing;
    size_t len;
};

// TODO: pull this out to common stuff for this project.
struct CG_StringRef
{
    CG_StringRef(const char *str)
    {
        this->str = str;
        this->len = strlen(str);
        this->bStrongRef = false;
    }

    CG_StringRef(CG_String &str)
    {
        this->str = str.backing;
        this->len = str.len;
        this->bStrongRef = true;
    }

    const char *str;
    size_t len;

    // NOTE: true when the string will not be dealloc.
    // this reference is ownership.
    bool bStrongRef;
};

struct CG_Value
{
    ppl_type valueKind = PPL_TYPE_UNKNOWN;
    union {
        ppl_type v_PplType;
        uint64_t v_Uint64;
    };
};

uint64_t ValueExtract_Uint64(CG_Value val)
{
    return val.v_Uint64;
}

ppl_type ValueExtract_PplType(CG_Value val)
{
    // TODO:
    return val.v_PplType;
}

struct CG_CompileTimeVarRecord
{
    CG_StringRef name;
    ppl_type type = PPL_TYPE_UNKNOWN;
    CG_Value val;
};

// Replace SpecialFilehandle with PFileWriter

void GenerateProgram(struct tree_node ast, PFileWriter &fileWriter);
static bool ExpressionVerifyConstant(struct tree_node *ast);
static CG_Value ConstantExpressionCompute(struct tree_node *ast);

// NOTE(Noah): Codegen assumes that the AST being passed has GNODE:program
// at the root.
// NOTE(Noah): It also assumes that semantic parsing has been done.
//  NOTE(Noah): ^ this is a big note.
void RunCodegen(struct tree_node ast, const char *outFile) {
    FILE *fileHandle = fopen(outFile, "w");
    PFileWriter fileWriter = PFileWriter(fileHandle); // takes ownership of file handle.
    GenerateProgram(ast, fileWriter);
}

// TODO: we really want something like ExpressionGatherInfo where it computes useful info
// such as what the total value kind of the expression is, but also what the actual value of
// the expression is. this kind of work is related and therefore we want to group this stuff.
// but it is also the case where we want the version that special cases just to get the kind
// of the expr, cuz that's less work than computing the total value.
//
// TODO: right now the TL_out mechanism isn't really fully coded. not really sure exactly what we
// want to do there.
static void ExpressionInferKind(struct tree_node *ast, ppl_type *kindOut, tree_node **TL_out)
{
    // ast node is
    // "[(import_expression)(data_pack)(span_expression)(assignment_exp)(conditional_exp)]",
    // "((op,,)(expression))*"

    tree_node *child = &ast->children[0];
    ppl_type &kind = *kindOut;

    kind = PPL_TYPE_UNKNOWN;

    // TODO: once again, we really want this to be a simple switch statement here.
    if ( strcmp(child->metadata.str, "import_expression") == 0 )
    {
        kind = PPL_TYPE_NAMESPACE;
    }
    else if ( strcmp(child->metadata.str, "data_pack") == 0 )
    {
        // ast node is
        // "(type)?\\{(statement)*[(statement)(statement_noend)]?\\}"
        if (child->childrenCount == 0)
        {
            // data pack is {}.
            kind = PPL_TYPE_UNKNOWN;
        }

        // TODO:
        kind = PPL_TYPE_UNKNOWN;
    }
    else if ( strcmp(child->metadata.str, "span_expression") == 0 )
    {
        kind = PPL_TYPE_SPAN;
    }
    else if ( strcmp(child->metadata.str, "assignment_exp") == 0 )
    {
        // TODO: it might be interesting to have a system where we can at runtime verify the structure
        // of the thing that was parsed. this way we know that the AST is the thing that we expect it to be.

        // ast node is
        // "(factor)[(op,=)(op,+=)(op,<<=)(op,>>=)(op,^=)(op,-=)(op,*=)(op,/=)(op,%=)(op,&=)(op,|=)](expression)"
        assert( child->childrenCount == 3 );
        tree_node *exp = &child->children[2];
        ExpressionInferKind(exp, &kind, TL_out);
    }
    else if ( strcmp(child->metadata.str, "conditional_exp") == 0 )
    {
        // TODO: verify that both sides of the conditional are the same type.

        // ast node is
        // "(logical_or_exp)(\\?(expression):(expression))?"
        tree_node *exp = (child->childrenCount == 1) ? &child->children[0] : &child->children[1];
        ExpressionInferKind(exp, &kind, TL_out);
    }
    else if (
    strcmp(child->metadata.str, "logical_or_exp") == 0 ||
    strcmp(child->metadata.str, "logical_and_exp") == 0 || 
    strcmp(child->metadata.str, "equality_exp") == 0 ||
    strcmp(child->metadata.str, "relational_exp") == 0 )
    {
        // ast node is
        // "(bitwise_or_exp)((op,&&)(bitwise_or_exp))*"
        if (child->childrenCount == 1 )
        {
            tree_node *exp = &child->children[0];
            ExpressionInferKind(exp, &kind, TL_out);
        }
        else
        {
            kind = PPL_TYPE_BOOL;
        }
    }
    else if ( strcmp(child->metadata.str, "bitwise_or_exp") == 0 || 
        strcmp(child->metadata.str, "bitwise_xor_exp") == 0 ||
        strcmp(child->metadata.str, "bitwise_and_exp") == 0 || 
        strcmp(child->metadata.str, "bitshift_exp") == 0 || 
        strcmp(child->metadata.str, "additive_exp") == 0 || 
        strcmp(child->metadata.str, "term") == 0)
    {
        // TODO: there might be some casting or whatever going on.
        // so we can't just take the first child.
        // if there are many operands to the |, need to find the largest
        // bitwidth one of em'.

        // ast node is
        //  "(bitwise_xor_exp)((op,|)(bitwise_xor_exp))*"
        //
        // "(bitwise_and_exp)((op,^)(bitwise_and_exp))*"
        //
        // "(equality_exp)((op,&)(equality_exp))*"
        tree_node *exp = &child->children[0];
        ExpressionInferKind(exp, &kind, TL_out);
    }
    else if ( strcmp(child->metadata.str, "factor") == 0 )
    {
        // ast node is
        /*        
        "["
            "((op,.)(object))"
            "(object)"
            "([(op,++)(op,--)(op,!)(op,-)(op,*)(op,@)(op,~)(\\((type)\\))](factor))"
            "(\\((expression)\\))"
        "]"
        */
       if ( child->childrenCount == 2 )
       {
            tree_node *op = &child->children[0];
            if ( op->type == AST_OP)
            {
                char opc = op->metadata.str[2];
                switch(opc)
                {
                    case '!':
                    kind = PPL_TYPE_BOOL;
                    break;
                    case '.':
                    case '+':
                    case '-':
                    case '~':
                    default: // TODO: need to handle pointer and deref.
                    {
                        tree_node *exp = &child->children[1];
                        ExpressionInferKind(exp, &kind, TL_out);
                    } break;    
                }
            }
            else
            {
                // this is a cast.
                tree_node *typeExpression = op;
                assert ( ExpressionVerifyConstant(typeExpression) );

                // TODO: might make this async in the future?
                auto val = ConstantExpressionCompute(typeExpression);
                //assert( val.valueKind == PPL_TYPE_TYPE );
                ppl_type valE = ValueExtract_PplType( val );
                kind = valE;
            }
       }
       else
       {
            // node is object or expression.
            tree_node *exp = &child->children[0];
            ExpressionInferKind(exp, &kind, TL_out);
       }
    }
    else if (strcmp(child->metadata.str, "object") == 0)
    {
        // ast node is
        // "[(function_call)(symbol)(literal)(type_literal)]", // beta
        // "[(op,++)(op,--)((op,[)(expression)(op,]))((op,.)(object))]" // alpha

        if (child->childrenCount == 1)
        {
            tree_node *child2 = &child->children[0];
            if ( child2->type == AST_INT_LITERAL )
            {
                kind = child2->metadata.valueKind;
                if (TL_out)  *TL_out = child2;
            }
            else if ( strcmp(child2->metadata.str, "type_literal") == 0 )
            {
#if 0
                tree_node *keyword = &child2->children[0].children[0];

                // TODO: this thing below is stupid and slow and ideally we should already have this information.
                kind = KeywordToPplType(keyword->metadata.str);
#endif
                kind = PPL_TYPE_TYPE;
                if (TL_out) *TL_out = &child2->children[0].children[0];
            }

            // TODO: handle infer from the return type of the function call.

            // TODO: handle variable lookup from symbol.
        }

        // TODO: handle the case where we infer from the member access at the end of the . chain.
        // TODO: handle the case where we do an array index. what is the kind of the thing in the array that we are indexing?
    }

    // TODO: handle object.
}

static bool ExpressionVerifyConstant(struct tree_node *ast)
{
    // TODO.
    return true;
}

static CG_Value ConstantExpressionCompute(struct tree_node *ast)
{
    auto val = CG_Value {};

    tree_node *TL;
    ppl_type kind;
    ExpressionInferKind(ast, &kind, &TL);

    val.valueKind = kind;

    // TODO: handle other cases. right now I'm being really lazy.
    if ( kind == PPL_TYPE_TYPE )
    {
        tree_node *keyword = TL;
        // TODO: this thing below is stupid and slow and ideally we should already have this information.
        val.v_PplType = KeywordToPplType(keyword->metadata.str);
    }
    else
    {
        switch(kind)
        {
            case PPL_TYPE_U8:
            case PPL_TYPE_U16:
            case PPL_TYPE_U32:
            case PPL_TYPE_U64:
            case PPL_TYPE_S8:
            case PPL_TYPE_S16:
            case PPL_TYPE_S32:
            case PPL_TYPE_S64:
            val.v_Uint64 = TL->metadata.num;
        }   
    }

    return val;
}

static void GenerateCompileTimeVarDecl(struct tree_node *ast, PFileWriter &fileWriter)
{
    // ast node is
    // "(route):"
    // "[((type)(op,=)(expression))(expression)]"
    // where route ast node is "(symbol):"
    tree_node *route = &ast->children[0];
    tree_node *varName = &route->children[0];

    tree_node *value;

    ppl_type type;

    // TODO: add a thing to assert the strong type with the actual expression type that
    // we infer.

    const bool bStrongTyped = ast->childrenCount > 2;

    if (bStrongTyped)
    {
        // here we have the strongly typed case.
        // ast node is ((type)(op,=)(expression))
        tree_node *typeExpression = &ast->children[1];
        assert ( ExpressionVerifyConstant(typeExpression) );

        // TODO: might make this async in the future?
        auto val = ConstantExpressionCompute(typeExpression);
        //assert( val.valueKind == PPL_TYPE_TYPE );
        ppl_type valE = ValueExtract_PplType( val );
        type = valE;

        value = &ast->children[3];
    }
    else
    {
        value = &ast->children[1];
    }

    assert ( ExpressionVerifyConstant(value) );
    auto val = ConstantExpressionCompute(value);

    if (!bStrongTyped)
    {
        type = val.valueKind;
    }

    fileWriter.write("label_");
    fileWriter.write((char*)varName->metadata.str);
    fileWriter.write(":\n");

    // TODO: implement other types.
    switch(type)
    {
        // TODO: proper handle the negative types.
        case PPL_TYPE_U8:
        case PPL_TYPE_U16:
        case PPL_TYPE_U32:
        case PPL_TYPE_U64:
        case PPL_TYPE_S8:
        case PPL_TYPE_S16:
        case PPL_TYPE_S32:
        case PPL_TYPE_S64:
        {
            // TODO: since the write func does not modify the string that it takes
            // in, we can make it take a const char *.
            fileWriter.write(".db ");

            // NOTE: the value extract is gonna work between any integer type,
            // so long as we have the correct bit width.
            uint64_t integerValue = ValueExtract_Uint64(val);

            char *integerString = SillyStringFmt("%u", integerValue); 

            fileWriter.write(integerString);
        }
        break;
    }


//    fileWriter.write( (char*)PplTypeToString(type) );
}

// NOTE: the goal of generate program should be to execute PASM assembly instructions.
// we'll then run those through our assembler to generate the actual .EXE 
void GenerateProgram(struct tree_node ast, PFileWriter &fileWriter)
{
    // ast node is "((compile_time_var_decl);?)*"

    // TODO: maybe we want to use the iterator pattern. that might make things nice.
    for ( uint32 i = 0; i < ast.childrenCount; i++)
    {
        tree_node child = ast.children[i];
        if (child.type == AST_GNODE)
        {
            // TODO: please almighty god, let's get rid of these strings.
            if (0 == strcmp(child.metadata.str, "compile_time_var_decl")) {
                GenerateCompileTimeVarDecl(&child, fileWriter);
                fileWriter.write("\n");
            }
        }
    }
}

#endif
