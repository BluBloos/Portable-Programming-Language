#ifndef CODEGEN_H
#define CODEGEN_H

#define PPL_TYPE_INTEGER_CASE case PPL_TYPE_U8:\
case PPL_TYPE_U16:\
case PPL_TYPE_U32:\
case PPL_TYPE_U64:\
case PPL_TYPE_S8:\
case PPL_TYPE_S16:\
case PPL_TYPE_S32:\
case PPL_TYPE_S64:

#define CG_MAX_TUPLE_ELEMS 10

// NOTE: CG = codegen

struct CG_SpanValue
{
    uint64_t begin;
    uint64_t end;
};

// TODO: needs more design.
struct CG_String
{
    char *backing;
    size_t len;
    
    const char *c_str()
    {
        return SillyStringFmt("%.*s", backing, len);
    };
};

// TODO: needs more design.
struct CG_StringRef
{
    CG_StringRef()
    {
        this->str = "";
        this->len = 0;

        // the string cannot be dealloc since it's in ROM.
        this->bStrongRef = true;
    }

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
    
    const char *Get()
    {
        // TODO: if bStrongRef is true, we want to assert that str is not a dangling pointer.
        // how can we do that?
        
        return str;
    }

    const char *str;
    size_t len;

    // NOTE: true when the string will not be dealloc.
    // this reference is ownership.
    bool bStrongRef;
};

static constexpr size_t CG_MAX_FUNCTION_PARAMETERS = 10;

struct CG_FunctionSignature
{
    ppl_type returnType;

    // TODO: we want more params.
    // TODO: function parameters can be functions.
    ppl_type params[CG_MAX_FUNCTION_PARAMETERS];
    CG_StringRef paramIdents[CG_MAX_FUNCTION_PARAMETERS];
    
    uint8_t paramCount;
};

template <typename T_ElementKind>
struct CG_Span {
    T_ElementKind *container;
    size_t         offset;
    size_t         size;

    CG_Span() : container(nullptr), offset(0), size(0) {}

    CG_Span(T_ElementKind *container, size_t size) : container(container), offset(0), size(size) {}

    T_ElementKind *Get() { return &this->container[offset]; }
};

struct CG_Function
{
    CG_Span<CG_FunctionSignature> signature;
    tree_node *code;

    CG_Function() : signature(), code(nullptr)
    {}
};

// TODO: this seems kind of similar to some of the structures that are generated in the earlier
// codegen passes. we could probably make these the same thing.
struct CG_Value
{
    ppl_type valueKind;
    union {
        ppl_type v_PplType;
        uint64_t v_Uint64;
        CG_Function v_CgFunction; // a function literal (function definition, if you will).
        CG_Span<CG_FunctionSignature> v_CgFunctionSignature;
        CG_String v_CgString;
        CG_SpanValue v_CgSpanValue;
    };
    
    CG_Value() : valueKind(PPL_TYPE_UNKNOWN), v_CgFunction() {}
};

enum CG_MemoryLocationType
{
    CG_MEMORY_LOCATION_STACK = 0
};

struct CG_MemoryLocation
{
    CG_MemoryLocationType type;
    union {
        const char *stackIdent;
    };
    
    CG_MemoryLocation() : type(CG_MEMORY_LOCATION_STACK), stackIdent("") {}
};

struct CG_Id
{
    const char *name;
    CG_MemoryLocation loc;
    CG_Value type;
};

CG_MemoryLocation CG_ResolveIdent(CG_Id &ident)
{
    return ident.loc;
}

// =====================================================

struct CG_Globals
{
    PPL_HashMapWithStringKey<CG_Value> metaVars;
    
    // map the string literal to the label as found in the .pasm codegen output.
    PPL_HashMapWithStringKey<const char *> stringLiterals;
    
    // this is the current program stack.
    PPL_HashMapWithStringKey<CG_Id> runtimeVars;
    
    // idents + data to emit in the readonly section of the .exe
    PPL_HashMapWithStringKey<CG_Value> binaryReadonly;

    // TODO: I'm finding in a lot of cases that the stretchy buffer is a concept used quite a bit in this codebase.
    // right now I denote these with the comment of `stretchy buffer`, but should prob make a dynamic array utility thing.
    
    // stretchy buffer.
    CG_FunctionSignature *funcSignatureRegistryScratch;
    
    uint64_t labelUID;
    
    CG_Globals() : funcSignatureRegistryScratch(nullptr), labelUID(0) {}
};

static CG_Globals s_cgGlobals;

CG_Globals *CG_Glob()
{
    return &s_cgGlobals;
}

void CG_Create()
{
    s_cgGlobals = {};
    
    // TODO: there's a fundamental issue where we push to this scratch while also
    // storing refs to the scratch somewhere else. if there is a realloc, those refs
    // go stale.
    //
    // for now the workaround is to simply alloc enough space to ensure that there is no
    // realloc.
    StretchyBuffer_Grow(s_cgGlobals.funcSignatureRegistryScratch, 100);
}

void CG_Release()
{
    auto g = CG_Glob();
    g->metaVars.~PPL_HashMapWithStringKey();
    g->stringLiterals.~PPL_HashMapWithStringKey();
    g->runtimeVars.~PPL_HashMapWithStringKey();
    g->binaryReadonly.~PPL_HashMapWithStringKey();
    StretchyBufferFree(g->funcSignatureRegistryScratch);
    
    // TODO: it's kind of annoying that we have to define the init value twice for this member of Glob.
    g->labelUID = 0;
}

// =====================================================

// TODO: we could easily replace the ValueExtract idea with a switch statement.

CG_SpanValue ValueExtract_CgSpan(CG_Value val)
{
    return val.v_CgSpanValue;
}

uint64_t ValueExtract_Uint64(CG_Value val)
{
    return val.v_Uint64;
}

ppl_type ValueExtract_PplType(CG_Value val)
{
    // TODO:
    return val.v_PplType;
}

CG_Value ValueConstruct_PplType(ppl_type type)
{
    CG_Value val;
    val.valueKind = PPL_TYPE_TYPE;
    val.v_PplType = type;
    return val;
}

CG_Function ValueExtract_CgFunction(CG_Value val)
{
    return val.v_CgFunction;
}

CG_Span<CG_FunctionSignature> ValueExtract_CgFunctionSignature(CG_Value val)
{
    return val.v_CgFunctionSignature;
}

CG_String ValueExtract_CgString(CG_Value val)
{
    return val.v_CgString;
}

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

static CG_Value TypeExpressionCompute(tree_node *ast)
{
    auto fnCheckIfTlTypeNode = [](tree_node *ast) -> bool
    {
        const char *str = ast->metadata.str;
        // TODO: once again, this will get very fast and less lame. once we switch to enum values.
        return ( strcmp(str, "function_type") == 0 || strcmp(str, "pointer_type") == 0 || strcmp(str, "array_type") == 0
                || strcmp(str, "type_literal") == 0 || strcmp(str, "qualifier") == 0);
    };

    // NOTE: ast node that we get is (type_literal) or (type).
    //
    // in the case of (type), we note that we can just traverse down the tree like we are below.
    // while the grammar allows for different things, any other case would be semantically invalid.
    while( !fnCheckIfTlTypeNode(ast) )
    {
        // TODO(user-error):
        assert(ast->childrenCount == 1);
        ast = &ast->children[0];
    }

    CG_Value val;
    val.valueKind = PPL_TYPE_TYPE;
    
    // ast node is "[(function_type)(pointer_type)(array_type)(type_keyword)((qualifier)(type))]".
    
    if (ast->childrenCount == 1)
    {
        tree_node *child = &ast->children[0];
        
        // TODO: once again, we really want to remove these debug string ideas!
        if (strcmp(child->metadata.str, "function_type") == 0)
        {
            // ast node is
            /*
                "(keyword=fn)"
                "\\("
                    "("
                        "(runtime_var_decl)(,(runtime_var_decl))*"
                    ")?"
                "\\)"
                "((op,->)(type))?"
             */

            assert(child->childrenCount >= 1);

            val.valueKind = PPL_TYPE_FUNC_SIGNATURE;
            ppl_type   returnType;
            tree_node *lastNode = &child->children[child->childrenCount - 1];

            CG_FunctionSignature newSig = {};

            for (uint32_t i = 0; i < child->childrenCount; i++) {
                tree_node *c = &child->children[i];
                // assert( strcmp(c->metadata.str, "runtime_var_decl") == 0 );

                // ast node is
                /*
                 "(route)"
                 "["
                     "("
                         "(type)"
                         "("
                             "(op,=)"
                             "[(op,?)(expression)]"
                         ")?"
                     ")"
                     "((op,=)(expression))"
                 "]"
                 */
                if (strcmp(c->metadata.str, "runtime_var_decl") == 0) {
                    tree_node *route   = &c->children[0];
                    tree_node *varName = &route->children[0];

                    // TODO: support default args, which is when the runtime_var_decl includes the expression.

                    assert(c->childrenCount >= 2);

                    tree_node *type = &c->children[1];

                    CG_Value val = TypeExpressionCompute(type);
                    assert(val.valueKind == PPL_TYPE_TYPE);  // TODO(user-error).

                    newSig.params[newSig.paramCount]        = ValueExtract_PplType(val);
                    newSig.paramIdents[newSig.paramCount++] = varName->metadata.str;
                    
                    assert(newSig.paramCount <= CG_MAX_FUNCTION_PARAMETERS);
                } else {
                    break;
                }
            }

            if ( strcmp(lastNode->metadata.str, "type") == 0 )
            {
                CG_Value val = TypeExpressionCompute(lastNode);
                assert(val.valueKind == PPL_TYPE_TYPE); // TODO(user-error).
                returnType = ValueExtract_PplType(val);
            }
            else
            {
                returnType = PPL_TYPE_VOID;
            }
  
            // TODO: we want to only add the signature to the scratch space in the case that it doesn't already exist.
            // TODO: looks like we might want a ValuePut_* or something. just to be symmetric with ValueExtract*
            newSig.returnType = returnType;
            CG_FunctionSignature &newSigRef = StretchyBufferPush( CG_Glob()->funcSignatureRegistryScratch, newSig );
            val.v_CgFunctionSignature = CG_Span<CG_FunctionSignature>( &newSigRef, 1 );
        }
        else if (strcmp(child->metadata.str, "pointer_type") == 0)
        {
            // TODO:
        }
        else if (strcmp(child->metadata.str, "array_type") == 0)
        {
            // TODO:
        }
        else if (strcmp(child->metadata.str, "type_keyword") == 0)
        {
            tree_node *keyword = &child->children[0];
            // TODO: this thing below is stupid and slow and ideally we should already have this information.
            val.v_PplType = KeywordToPplType(keyword->metadata.str);
        }
    }
    else
    {
        PPL_TODO;
    }

    return val;
}

// NOTE: this is generated to varying degrees from the execution of ExpressionInferInfo.
// sometimes in order to infer the kind, we need to go farther in our computation. whatever
// info we collect, we don't want to throw that away. that's the idea with this struct.
struct CG_ExpressionInfo
{
    // the top-level node that we can use after inferring the type to do further processing.
    // the "further processing" is typically related to the value of the expression.
    // this is useful at least for the reason that our AST trees are too deep in some cases; via this
    // we can make them shorter.
    tree_node *TL;

    ppl_type kind;

    // when the resulting ppl_type is PPL_TYPE_FUNC, we'll have also gathered the sig.
    union {
        CG_Value funcSig;
        
        // TODO: there could be an arbitrary amount of casts.
        //CG_Value castType;
    };
    
    CG_ExpressionInfo() : funcSig() {}
};

// TODO: right now the TL_out mechanism isn't really fully coded. not really sure exactly what we
// want to do there.
static void ExpressionInferInfo(struct tree_node *ast, CG_ExpressionInfo *infoOut)
{
    // ast node is
    // "[(import_expression)(data_pack)(span_expression)(assignment_exp)(conditional_exp)]",
    // "((op,,)(expression))*"

    tree_node *child = &ast->children[0];
    ppl_type &kind = infoOut->kind;

    kind = PPL_TYPE_UNKNOWN;

    // TODO: once again, we really want this to be a simple switch statement here.
    // we can get that going by removing all this debug string crap.
    
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

        tree_node *fc = &child->children[0];

        if ( strcmp(fc->metadata.str, "type") == 0 )
        {
            // if the value of the type is a function signature, then the type of the expression
            // is a function type.
            //
            // otherwise, the type is gonna be a type literal.
            //
            // consider e.g. if the type exp value is simply struct, then the expression type is TypeInfo.
            //
            // recall that not all typed data packs make sense. something like e.g. int {}.
            // that's nonsense.
            
            // we have to crawl the tree to get to type_literal, then can check what kind of type literal we are looking at.
            
            auto val = TypeExpressionCompute(fc);
            if (val.valueKind == PPL_TYPE_FUNC_SIGNATURE)
            {
                infoOut->funcSig = val;
                infoOut->TL = child;
                
                kind = PPL_TYPE_FUNC;
            }
            else
            {
                // something that ends up in this branch e.g. would be structs.
                PPL_TODO;
            }
        }
    }
    else if ( strcmp(child->metadata.str, "span_expression") == 0 )
    {
        kind = PPL_TYPE_SPAN;
        if (infoOut)  infoOut->TL = child;
    }
    else if ( strcmp(child->metadata.str, "assignment_exp") == 0 )
    {
        // TODO: it might be interesting to have a system where we can at runtime verify the structure
        // of the thing that was parsed. this way we know that the AST is the thing that we expect it to be.

        // ast node is
        // "(factor)[(op,=)(op,+=)(op,<<=)(op,>>=)(op,^=)(op,-=)(op,*=)(op,/=)(op,%=)(op,&=)(op,|=)](expression)"
        assert( child->childrenCount == 3 );
        tree_node *exp = &child->children[2];
        ExpressionInferInfo(exp, infoOut);
    }
    else if ( strcmp(child->metadata.str, "conditional_exp") == 0 )
    {
        // TODO: verify that both sides of the conditional are the same type.

        // ast node is
        // "(logical_or_exp)(\\?(expression):(expression))?"
        tree_node *exp = (child->childrenCount == 1) ? &child->children[0] : &child->children[1];
        ExpressionInferInfo(exp, infoOut);
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
            ExpressionInferInfo(exp, infoOut);
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
        ExpressionInferInfo(exp, infoOut);
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
                        ExpressionInferInfo(exp, infoOut);
                    } break;    
                }
            }
            else
            {
                // this is a cast.
                tree_node *typeExpression = op;
                assert ( ExpressionVerifyConstant(typeExpression) );

                auto val = TypeExpressionCompute(typeExpression);
                assert( val.valueKind == PPL_TYPE_TYPE );
                ppl_type valE = ValueExtract_PplType( val );
                kind = valE;
            }
       }
       else
       {
            // node is object or expression.
            tree_node *exp = &child->children[0];
            ExpressionInferInfo(exp, infoOut);
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
                if (infoOut)  infoOut->TL = child2;
            }
            else if ( child2->type == AST_STRING_LITERAL )
            {
                // TODO: we need to support when the compile-time variable is a string.
                PPL_TODO;
            }
            else if ( strcmp(child2->metadata.str, "type_literal") == 0 )
            {
                kind = PPL_TYPE_TYPE;
                if (infoOut) infoOut->TL = child2;
            }

            // TODO: handle infer from the return type of the function call.

            // TODO: handle variable lookup from symbol.
        }

        // TODO: handle the case where we infer from the member access at the end of the . chain.
        // TODO: handle the case where we do an array index. what is the kind of the thing in the array that we are indexing?
    }
    else
    {
        PPL_TODO;
    }
}

// verify that the type is simple. i.e. that it can fit in a register.
// and also that it can be used as the value within a CG_Value of type
// PPL_TYPE_TYPE.
static bool TypeVerifySimple(ppl_type type)
{
    switch(type)
    {
        PPL_TYPE_INTEGER_CASE
        {
            return true;
        }
        default:
            PPL_TODO;
    }
    return false;
}

static bool ExpressionVerifyConstant(struct tree_node *ast)
{
    // TODO.
    return true;
}

// TODO: this function may grow the function signature registry scratch space.
// maybe that changes in a future architecture of this codebase.
static CG_Value ConstantExpressionCompute(struct tree_node *ast)
{
    auto val = CG_Value {};

    CG_ExpressionInfo info;

    ExpressionInferInfo(ast, &info);
    
    ppl_type &kind = info.kind;
    tree_node *TL = info.TL;
    val.valueKind = kind;

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
            break;
        case PPL_TYPE_TYPE:
        val = TypeExpressionCompute(TL);
            break;
        case PPL_TYPE_FUNC:
        {
            CG_Function newFunc;
            newFunc.code = TL;
            newFunc.signature = ValueExtract_CgFunctionSignature(info.funcSig);
            
            val.v_CgFunction = newFunc;
        } break;
        case PPL_TYPE_SPAN:
        {
            CG_SpanValue span;
            
            assert( TL->type == AST_GNODE && strcmp(TL->metadata.str, "span_expression") == 0 );

            // ast node is "(literal)[(op,..=)(op,..<)](literal)"
            assert(TL->childrenCount == 3);
            tree_node *begin = &TL->children[0];
            tree_node *op = &TL->children[1];
            tree_node *end = &TL->children[2];
            
            assert(op->type == AST_OP);
            
            if ( begin->type == AST_INT_LITERAL && end->type == AST_INT_LITERAL)
            {
                span.begin = begin->metadata.num;
                span.end = end->metadata.num;
                char opChar = op->metadata.str[4];
                if (opChar == '=')
                {
                    span.end += 1;
                    if(span.end == 0)
                    {
                        // not sure right now what to do in the wraparound case.
                        PPL_TODO;
                    }
                }
                else if (opChar == '<')
                {
                    // nothing to do here.
                }
                else
                {
                    PPL_TODO;
                }
            }
            else
            {
                // I'm not even sure if we allow such spans?
                // at any rate, not implementing this path right now.
                PPL_TODO;
            }
            
            val.v_CgSpanValue = span;
        } break;
        default:
            PPL_TODO;
            break;
    }

    return val;
}

static void RecordCompileTimeVarDecl(struct tree_node *ast, PFileWriter &fileWriter)
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

        auto val = TypeExpressionCompute(typeExpression);
        
        if (val.valueKind == PPL_TYPE_FUNC_SIGNATURE)
        {
            // NOTE: this is valid syntax to define a function but since it's odd,
            // we won't support rn.
            PPL_TODO;
        }
        
        type = val.valueKind;
        value = &ast->children[3];
    }
    else
    {
        value = &ast->children[1];
    }

    // TODO: might make this async in the future?
    // especially once the #run is added.
    assert ( ExpressionVerifyConstant(value) );
    auto val = ConstantExpressionCompute(value);

    if (!bStrongTyped)
    {
        type = val.valueKind;
    }
    
    // NOTE: record the variable.
    CG_Glob()->metaVars.put( varName->metadata.str, val );
    
    // NOTE: emit a comment if the switch below doesn't emit the var, just
    // so that we don't get lots of empty space.
    fileWriter.write("; recording ");
    fileWriter.write((char*)varName->metadata.str);
}

struct CG_Tuple
{
    tree_node *elems[CG_MAX_TUPLE_ELEMS];
    uint32_t elemCount;
    
    CG_Tuple() : elemCount(0) {}
};

// NOTE: getting the tuple is as simple as flattening the tree.
void CollectTupleFromExpression(struct tree_node *exp, CG_Tuple *builder)
{
    if (exp->childrenCount >= 3)
    {
        builder->elems[builder->elemCount++] = &exp->children[0];
        assert(builder->elemCount <= CG_MAX_TUPLE_ELEMS);

        assert(exp->children[1].type == AST_OP);
        CollectTupleFromExpression(&exp->children[2], builder);
    }
    else if (exp->childrenCount == 1)
    {
        builder->elems[builder->elemCount++] = exp;
        assert(builder->elemCount <= CG_MAX_TUPLE_ELEMS);
    }
    else
    {
        PPL_TODO;
    }
    
    return;
}

// TODO: there are lot of places in this codegen pass where we do the recursion, but that's not good.
// that kind of idea does not scale well and is generally not performant.
// replace the recursion with the usage of a stack idea.

// TODO: handle when the result of the expression is wider than 64 bits, or is a float. for now, we just do integers in a single register.
// NOTE: this will generate the code from an expression to build the result for immediate usage.
// the result of the expression will be stored to r2 (pasm).
void GenerateExpressionImmediate(struct tree_node *ast, PFileWriter &fileWriter, uint32_t indentation, const char *parentFuncName)
{
    // TODO:
    assert(indentation == 1);
    //const char *indentationStr = "\t";
    // TODO:
    const char *indentationStr = "";
    
    // ast node is
    /*
     "[(import_expression)(data_pack)(span_expression)(assignment_exp)(conditional_exp)]",
     "((op,,)(expression))*"
     */
    
    // TODO: handle the Tuple expressions more generally. right now we only handle for function params.
    assert(ast->childrenCount >= 1);
    
    auto c = &ast->children[0];
    
    if (c->type == AST_STRING_LITERAL)
    {
        
        CG_String newStr;
        newStr.backing = (char *)c->metadata.str; // TODO: discarding qualifiers.
        newStr.len = strlen(newStr.backing);
        
        CG_Value val;
        val.valueKind = PPL_TYPE_STRING;
        val.v_CgString = newStr;

        // when we use the metadata.str as the key, it's OK. it's a null terminated string.
        // the problem is that it cannot be used as a label within the ASM source.
        
        // what we can do is have a label generator. this can give back `msg%d` with increasing integer
        // values. that should be good for many strings.
        //
        // we can create a hash map and use metadata.str as the key, with the `msg%d` label as the value.
        // we'll use the `msg%d` as the key for the variable in the metavars table.
        //
        // so this is a nice little indirection we've got going here.
        
        const char *label;
        if (!CG_Glob()->stringLiterals.get(c->metadata.str, &label))
        {
            // the label doesn't exist yet - generate it.
            auto s = SillyStringFmt("msg%u", CG_Glob()->labelUID++ );
            
            
            // TODO: I'm pretty sure that this currently causes issues.
            // where if we keep the compiler instance running, we are going to have memory allocate
            // and never dealloc. we also might overflow the arena.
            label = MEMORY_ARENA.StringAlloc((char *)s);
            
            // record the mapping from string literal to the label.
            CG_Glob()->stringLiterals.put( c->metadata.str, label );
            
            // record the metavariable.
            CG_Glob()->binaryReadonly.put( label, val );
        }
        
        auto s = SillyStringFmt("%smov r2, %s\n", indentationStr, label);
        fileWriter.write(s);
    }
    else if (c->type == AST_INT_LITERAL)
    {
        // TODO: we really need to care about the proper typing of things.
        c->metadata.num; // int64.
        auto s = SillyStringFmt("%smov r2, %d\n", indentationStr, c->metadata.num);
        fileWriter.write(s);
    }
    else if (strcmp(c->metadata.str, "assignment_exp") == 0)
    {
        PPL_TODO;
    }
    else if (strcmp(c->metadata.str, "additive_exp") == 0)
    {
        // ast node is  "(term)([(op,+)(op,-)](term))*".
        if (c->childrenCount > 1)
        {
            tree_node *term1 = &c->children[0];
            GenerateExpressionImmediate(term1, fileWriter, indentation, parentFuncName);
            
            // save the last computed term into r3.
            auto s = SillyStringFmt("%smov r3, r2\n", indentationStr);
            fileWriter.write(s);

            for ( uint32_t i = 1; i < c->childrenCount; i += 2 )
            {
                tree_node *op = &c->children[i];
                tree_node *term2 = &c->children[i+1];
                
                assert(op->type == AST_OP);
                char opChar = op->metadata.str[2];
                
                GenerateExpressionImmediate(term2, fileWriter, indentation, parentFuncName);

                char *s;
                if (opChar == '+')
                    s = SillyStringFmt("%sadd r3, r2\n", indentationStr);
                else if (opChar == '-')
                    s = SillyStringFmt("%ssub r3, r2\n", indentationStr);
                else
                    PPL_TODO;

                fileWriter.write(s);
            }
            
            // output the result.
            s = SillyStringFmt("%smov r2, r3\n", indentationStr);
            fileWriter.write(s);
        }
        else
        {
            GenerateExpressionImmediate(c, fileWriter, indentation, parentFuncName);
        }
    }
    else if (
             strcmp(c->metadata.str, "logical_or_exp") == 0 ||
             strcmp(c->metadata.str, "logical_and_exp") == 0 ||
             strcmp(c->metadata.str, "equality_exp") == 0 ||
             strcmp(c->metadata.str, "relational_exp") == 0 ||
             strcmp(c->metadata.str, "bitwise_or_exp") == 0 ||
             strcmp(c->metadata.str, "bitwise_xor_exp") == 0 ||
             strcmp(c->metadata.str, "bitwise_and_exp") == 0 ||
             strcmp(c->metadata.str, "bitshift_exp") == 0 ||
             strcmp(c->metadata.str, "term") == 0 ||
             strcmp(c->metadata.str, "factor") == 0 ||
             strcmp(c->metadata.str, "object") == 0
             )
    {
        GenerateExpressionImmediate(c, fileWriter, indentation, parentFuncName);
    }
    else if (strcmp(c->metadata.str, "function_call") == 0)
    {
        tree_node *callee = &c->children[0];
        assert(callee->type == AST_SYMBOL);
        auto calleeName = callee->metadata.str;
        
        // collect the arguments.
        CG_Tuple args;
        CollectTupleFromExpression(&c->children[1], &args);
        
        // generate the args.
        // TODO: this can of course be more efficient. I'm perfectly fine at this stage of the project to emit poor asm.
        // the goal right now is to just get things working.
        for (uint32_t i = 1; i < args.elemCount; i++)
        {
            GenerateExpressionImmediate(args.elems[i], fileWriter, indentation, parentFuncName);
            auto s = SillyStringFmt("%smov r%d, r2\n", indentationStr, i+2);
            fileWriter.write(s);
        }
        
        // NOTE: the leftmost one goes into r2.
        GenerateExpressionImmediate(args.elems[0], fileWriter, indentation, parentFuncName);
        
        auto s = SillyStringFmt("%scall %s(", indentationStr, calleeName);
        fileWriter.write(s);
        
        for (uint32_t i = 0; i < args.elemCount; i++)
        {
            const char * s;
            if (i == args.elemCount - 1)
                s = SillyStringFmt("r%d", 2+i);
            else
                s = SillyStringFmt("r%d, ", 2+i);
            fileWriter.write((char*)s);
        }
        
        fileWriter.write(")\n");
        
    }
    else if ( c->type == AST_SYMBOL )
    {
        // TODO: handle lookup for runtime variables. right now we just lookup the compile-time ones.
        auto identName = c->metadata.str;
        CG_Value val;
        CG_Id ident;
        if ( CG_Glob()->metaVars.get( identName, &val) )
        {
            switch(val.valueKind)
            {
                    // TODO: handle types other than integer.
                    PPL_TYPE_INTEGER_CASE
                    {
                        auto s = SillyStringFmt(
                                                "%smov r2, %u\n",
                                                indentationStr, ValueExtract_Uint64(val));
                        fileWriter.write(s);
                    }break;
                default:
                    PPL_TODO;
            }
        }
        else if ( CG_Glob()->runtimeVars.get( identName, &ident ) )
        {
            CG_MemoryLocation loc = CG_ResolveIdent(ident);
            
            // TODO: do the proper variable scoping. also dealloc the variables.
            // the funny is that I can get the current desired next program to work
            // by ignoring this kind of thing.
            //   we could do a loc.scope where .scope is an AST node and we check for
            //   a common ancestor of currScope to the .scope.
            
            switch(loc.type)
            {
                case CG_MEMORY_LOCATION_STACK:
                {
                    // TODO: when we look at doing the proper typing stuff;
                    // that means constructing a move operation into the correct sized register.
                    auto s = SillyStringFmt( "%smov r2, %s\n", indentationStr, loc.stackIdent );
                    fileWriter.write(s);
                } break;
                default:
                    PPL_TODO;
            }
        }
        else
        {
            // TODO: want a user error. I'm thinking something like CgEmitError;
            // this is the ubiquitous error for the undeclared identifier.
            PPL_TODO;
        }
    }
    else if (strcmp(c->metadata.str, "conditional_exp") == 0)
    {
#define FUNC_TERN_LABEL_FMT          "label_%s_tern_%d"
#define FUNC_TERN_LABEL_NO_LABEL_FMT "%s_tern_%d"
    
        if (c->childrenCount == 1)
        {
            GenerateExpressionImmediate(&c->children[0], fileWriter, indentation, parentFuncName);
        }
        else
        {
            fileWriter.write("; conditional_exp\n");
            auto cond = &c->children[0];
            auto a = &c->children[1];
            auto b = &c->children[2];
            
            uint64_t label1 = CG_Glob()->labelUID++;
            uint64_t label2 = CG_Glob()->labelUID++;
            
            
            GenerateExpressionImmediate(cond, fileWriter, indentation, parentFuncName);
            
            auto s = SillyStringFmt(
                                    "%sbgt r2, 0, " FUNC_TERN_LABEL_NO_LABEL_FMT "\n",
                                    indentationStr,
                                    parentFuncName, label1
                                    );
            fileWriter.write(s);
            
            GenerateExpressionImmediate(b, fileWriter, indentation, parentFuncName);
            
            s = SillyStringFmt(
                               "%sbr " FUNC_TERN_LABEL_NO_LABEL_FMT "\n"
                               FUNC_TERN_LABEL_FMT ":\n",
                               indentationStr,
                               parentFuncName, label2,
                               parentFuncName, label1);
            
            fileWriter.write(s);

            GenerateExpressionImmediate(a, fileWriter, indentation, parentFuncName);
            
            s = SillyStringFmt(FUNC_TERN_LABEL_FMT ":\n", parentFuncName, label2);
            fileWriter.write(s);
        }
#undef FUNC_TERN_LABEL_FMT
#undef FUNC_TERN_LABEL_NO_LABEL_FMT
    }
}

#define FUNC_END_LABEL_FMT          "label_%s_end"
#define FUNC_END_LABEL_NO_LABEL_FMT "%s_end"

void GenerateStatement(struct tree_node *ast, PFileWriter &fileWriter, uint32_t indentation, const char *parentFuncName)
{
    // TODO: support arbitrary indentations.
    assert(indentation == 1);
    
    // TODO:
    //const char *indentationStr = "\t";
    const char *indentationStr = "";
    
    // ast node is either (statement) or (statement_noend).

    // for statement, we have that
    //  "["
    //  "((compile_time_var_decl);?)"
    //  "(untyped_data_pack)"
    //  ";"
    //  "([(return_statement)(runtime_var_decl)(expression)(keyword=fall)`(keyword=break)`(keyword=continue)`];)"
    //  "(if_statement)(while_statement)(for_statement)(switch_statement)
    //  "]"
    
    assert(ast->childrenCount == 1);
    
    // TODO: once again, we want to get rid of this string stuff.
    auto c = &ast->children[0];
    auto n = c->metadata.str;
    if (strcmp(n, "compile_time_var_decl") == 0 )
    {
        PPL_TODO;
    }
    else if (strcmp(n, "runtime_var_decl") == 0 )
    {
        // ast node is
        /*
         "(route)"  =  "(symbol):"
         "["
             "("
                 "(type)"
                 "("
                     "(op,=)"
                     "[(op,?)(expression)]"
                 ")?"
             ")"
             "((op,=)(expression))"
         "]"
         */
        
        // TODO: we can have a helper to get the variable name out of the route.
        assert( c->childrenCount >= 2 );
        tree_node *route = &c->children[0];
        assert( route->childrenCount >= 1 );
        tree_node *varName = &route->children[0];
        assert( varName->type == AST_SYMBOL );
        
        tree_node *maybeType = &c->children[1];
        
        CG_Value typeValue;
        
        tree_node *firstVal = nullptr;
        
        if (maybeType->type == AST_OP)
        {
            assert( c->childrenCount >= 3 );
            tree_node *exp = &c->children[2];

            CG_ExpressionInfo info;
            ExpressionInferInfo(exp, &info);
            
            assert( TypeVerifySimple(info.kind) );
            
            firstVal = exp;
            typeValue = ValueConstruct_PplType( info.kind ) ;
        }
        else if (strcmp(maybeType->metadata.str, "type") == 0)
        {
            typeValue = TypeExpressionCompute(maybeType);
            
            if (c->childrenCount >= 4)
            {
                tree_node *equalsOp = &c->children[2];
                tree_node *exp = &c->children[3];
                assert( equalsOp->type == AST_OP );
                firstVal = exp;
            }
        }
        else
        {
            PPL_TODO;
        }
        
        auto key = varName->metadata.str;
        
        CG_Id ident;
        ident.name = key;
        ident.type = typeValue;
        ident.loc.type = CG_MEMORY_LOCATION_STACK;
        ident.loc.stackIdent = key;
        
        // generate.
        CG_Glob()->runtimeVars.put( key, ident );
        
        if ( ident.type.valueKind != PPL_TYPE_TYPE )
        {
            // for now just expecting simple types.
            PPL_TODO;
        }

        const char *pasmType = PplTypeToPasmHumanReadable( ValueExtract_PplType(typeValue) );
        auto s = SillyStringFmt("%s.let %s %s\n", indentationStr, pasmType, ident.loc.stackIdent);
        fileWriter.write(s);
        
        if (firstVal)
        {
            // the variable decl also includes assignment, so do that.
            if (firstVal->type == AST_OP)
            {
                assert( strcmp(firstVal->metadata.str, "op?") == 0 );
            }
            else
            {
                assert( strcmp(firstVal->metadata.str, "expression") == 0 );
                
                GenerateExpressionImmediate(firstVal, fileWriter, indentation, parentFuncName);
                
                auto s = SillyStringFmt("%smov %s, r2\n", indentationStr, ident.loc.stackIdent);
                fileWriter.write(s);
            }
        }
        else
        {
            // we need to default initialize the variable. the `?` explicit
            // non-init idea is handled when firstVal != nullptr.
            
            // TODO: we could do the XOR here, but backend does not support yet.
            // fileWriter.write("%sxor %s, %s\n", indentationStr, ident.loc.stackIdent, ident.loc.stackIdent);
            auto s = SillyStringFmt("%smov %s, 0\n", indentationStr, ident.loc.stackIdent);
            fileWriter.write(s);
        }
        
    }
    else if (strcmp(n, "untyped_data_pack") == 0 )
    {
        PPL_TODO;
    }
    else if (strcmp(n, "return_statement") == 0 )
    {
        // ast node is "(keyword=return)(expression)"
        assert(c->childrenCount == 1);
        auto exp = &c->children[0];
        
        fileWriter.write("; return_statement\n");

        // NOTE: handle the expresssion if a value needs to be return.
        GenerateExpressionImmediate(exp, fileWriter, 1, parentFuncName);
        
        auto s = SillyStringFmt("%sbr " FUNC_END_LABEL_NO_LABEL_FMT "\n", indentationStr, parentFuncName);
        fileWriter.write(s);
    }
    else if (strcmp(n, "expression") == 0 )
    {
        // NOTE: this will generate into r2, but we don't care about the value.
        // we still have to emit instructions for this since the expression could do
        // something like a function call with side-effects.
        GenerateExpressionImmediate(c, fileWriter, indentation, parentFuncName);
    }
    else if (strcmp(n, "if_statement") == 0 )
    {
#define FUNC_IF_LABEL_NO_LABEL_FMT "%s_if_%d"
#define FUNC_IF_LABEL_FMT          "label_%s_if_%d"
        // ast node is
        // "(keyword=if)(expression)[;(keyword=then)](statement)((keyword=else)(statement))?"
        assert(c->childrenCount >= 2);
        tree_node *cond = &c->children[0];
        tree_node *body = &c->children[1];
        
        uint64_t label1 = CG_Glob()->labelUID++;
        uint64_t label2 = CG_Glob()->labelUID++;
        
        GenerateExpressionImmediate(cond, fileWriter, indentation, parentFuncName);
        
        auto s = SillyStringFmt(
                                "%sbgt r2, 0, " FUNC_IF_LABEL_NO_LABEL_FMT "\n",
                                indentationStr,
                                parentFuncName, label1
                                );
        fileWriter.write(s);
        
        // the code that happens for the false condition.
        if (c->childrenCount >= 3)
        {
            tree_node *body_else = &c->children[2];
            GenerateStatement(body_else, fileWriter, indentation, parentFuncName);
        }
        
        s = SillyStringFmt(
                           "%sbr " FUNC_IF_LABEL_NO_LABEL_FMT "\n"
                           FUNC_IF_LABEL_FMT ":\n",
                           indentationStr,
                           parentFuncName, label2,
                           parentFuncName, label1);
        
        fileWriter.write(s);

        // the code that happens for the true condition.
        GenerateStatement(body, fileWriter, indentation, parentFuncName);
        
        s = SillyStringFmt(FUNC_IF_LABEL_FMT ":\n", parentFuncName, label2);
        fileWriter.write(s);
#undef FUNC_IF_LABEL_NO_LABEL_FMT
#undef FUNC_IF_LABEL_FMT
    }
    else if ( strcmp(n, "while_statement") == 0 )
    {
        PPL_TODO;
    }
    else if ( strcmp(n, "for_statement") == 0 )
    {
#define FUNC_FOR_LABEL_NO_LABEL_FMT "%s_for_%d"
#define FUNC_FOR_LABEL_FMT          "label_%s_for_%d"
        // ast node is
        
        // "(keyword=for)
        // (
        //   (symbol)
        //   (,(symbol))*
        //   (keyword=in)
        // )?
        
        // NOTE: the first group is optional since they might not define variable names to recieve
        // the stuff. in that case, there is an implicit name of `it`.
        
        assert( c->childrenCount >= 2 );
        
        tree_node *maybeCond = &c->children[0];
        tree_node *maybeBody = &c->children[1];
        
        if ( maybeCond->type == AST_GNODE && strcmp(maybeCond->metadata.str, "expression") == 0 )
        {
            CG_ExpressionInfo info;
            ExpressionInferInfo(maybeCond, &info);
            
            if (info.kind != PPL_TYPE_SPAN)
            {
                // for now, we are only handling loops over a span.
                PPL_TODO;
            }
            
            // TODO: there is a double call to ExpressionInferInfo here.
            // this is because ConstantExpressionCompute internally calls ExpressionInferInfo.
            auto val = ConstantExpressionCompute(maybeCond);
            CG_SpanValue span = ValueExtract_CgSpan(val);
            
            uint64_t label1 = CG_Glob()->labelUID++;
            uint64_t label2 = CG_Glob()->labelUID++;
            uint64_t label3 = CG_Glob()->labelUID++; // loop counter temporary.

            //  TODO:
            // init the loop counter.
            auto s = SillyStringFmt("%s.let uint64 " FUNC_FOR_LABEL_NO_LABEL_FMT "\n",
                               indentationStr, parentFuncName, label3);
            fileWriter.write(s);
            s = SillyStringFmt("%smov " FUNC_FOR_LABEL_NO_LABEL_FMT ", %u\n",
                               indentationStr, parentFuncName, label3, span.begin);
            fileWriter.write(s);
            
            // emit loop begin label.
            s = SillyStringFmt(FUNC_FOR_LABEL_FMT ":\n",
                               parentFuncName, label1);
            fileWriter.write(s);
            
            // check.
            s = SillyStringFmt("%sbge " FUNC_FOR_LABEL_NO_LABEL_FMT ", %u, " FUNC_FOR_LABEL_NO_LABEL_FMT "\n",
                               indentationStr, parentFuncName, label3, span.end,
                               parentFuncName, label2);
            fileWriter.write(s);
            
            GenerateStatement(maybeBody, fileWriter, indentation, parentFuncName);
            
            // loop end op.
            s = SillyStringFmt("%sadd " FUNC_FOR_LABEL_NO_LABEL_FMT ", 1\n",
                               indentationStr, parentFuncName, label3);
            fileWriter.write(s);

            // go to loop top.
            s = SillyStringFmt("%sbr " FUNC_FOR_LABEL_NO_LABEL_FMT "\n",
                               indentationStr,
                               parentFuncName, label1);
            fileWriter.write(s);
            
            // emit loop end label.
            s = SillyStringFmt(FUNC_FOR_LABEL_FMT ":\n",
                               parentFuncName, label2);
            fileWriter.write(s);
            
            // destroy the loop counter.
            s = SillyStringFmt("%s.unlet uint64 " FUNC_FOR_LABEL_NO_LABEL_FMT "\n",
                               indentationStr, parentFuncName, label3);
            fileWriter.write(s);
        }
        else
        {
            // for now, let's ignore the `in` syntax.
            PPL_TODO;
        }
        
        // (expression)
        // [;(keyword=do)]
        // (statement)"
#undef FUNC_FOR_LABEL_NO_LABEL_FMT
#undef FUNC_FOR_LABEL_FMT
    }
    else if ( strcmp(n, "switch_statement") == 0 )
    {

        // TODO: for switch statement support we want to emit a jump table.
        // the idea here is that we'll emit a table where the elements are pointers and the index
        // is the enum value. the table is actually going to be a list of sequential instructions
        // that jump to the case code sections.
        // we'll use the enum value to jump into that instruction list. so it's a double jump. yahoo!
        // fallthrough is as simple as emit a noop where the table jump instr would have been. so we just
        // fall in the jump instr below, or out of the table entirely.

        PPL_TODO;        
    }
    else
    {
        // TODO: handle the keywords of fall, break, continue.
        PPL_TODO;
    }
}

uint8_t CG_ConvertEscapedChar(uint8_t c)
{
    switch(c)
    {
        case 'n':
            c = 10;
            break;
        case '\\':
            break;
        default:
            PPL_TODO;
    }
    return c;
}

// NOTE: the goal of generate program should be to generate our PASM IR.
// we'll then run those through the assembler component to generate the actual .EXE.
void GenerateProgram(struct tree_node ast, PFileWriter &fileWriter)
{
    // ast node is "((compile_time_var_decl);?)*"

    // TODO: we want to only include these function headers if the user requests them.
    fileWriter.write(
        ".extern p_decl void ppl_console_print(int64, []int64)\n"
        ".extern p_decl void ppl_exit(int32)\n\n"
         );

    // TODO: maybe we want to use the iterator pattern. that might make things nice.
    for ( uint32 i = 0; i < ast.childrenCount; i++)
    {
        tree_node child = ast.children[i];
        if (child.type == AST_GNODE)
        {
            // TODO: please almighty god, let's get rid of these strings.
            if (0 == strcmp(child.metadata.str, "compile_time_var_decl")) {
                
                // NOTE: Record is going to store the existence of the compile-time variables
                // in a hash map for later re-use. we can access these via their identifiers.
                //
                // the Generate step will emit the ROM part of the decl. any code will not be emit.
                RecordCompileTimeVarDecl(&child, fileWriter);
                fileWriter.write("\n");
            }
        }
    }
    
    fileWriter.write("\n.section code\n");
    
    // TODO: need to impl additional operators on the iterator thing.
    // NOTE: we must ensure that whilst iterating over the hashmap that we do not insert
    // additional items into it.
    for ( auto it = CG_Glob()->metaVars.begin(); it != CG_Glob()->metaVars.end(); it++)
    {
        PPL_HashMapWithStringKey_Element<CG_Value> tableElem = *it;

        if (tableElem.value.valueKind == PPL_TYPE_FUNC)
        {
            CG_Function func = ValueExtract_CgFunction(tableElem.value);
            CG_FunctionSignature *sigRef = func.signature.Get();

            const char *pasmStr = PplTypeToPasmHumanReadable(sigRef->returnType);

            // TODO: decompose the args.
            const char *s = SillyStringFmt(".def %s %s(", pasmStr, tableElem.key);
            fileWriter.write((char*)s);
            
            for (uint32_t i = 0; i < sigRef->paramCount; i++)
            {
                const char *s = PplTypeToPasmHumanReadable(sigRef->params[i]);
                const char *fs;
                if (i == sigRef->paramCount - 1)
                    fs = SillyStringFmt("%s %s", s, sigRef->paramIdents[i].Get());
                else
                    fs = SillyStringFmt("%s %s, ", s, sigRef->paramIdents[i].Get());
                fileWriter.write((char*)fs);
            }
            
            fileWriter.write(")\n");

            tree_node *funcCode = func.code;
            
            // TODO: we need to emit the register saving.
            // right now it's okay we are not doing it because there is only the main function.
            
            // NOTE: we start at node 1 since we know that for functions, the first node is the (type) node.
            // we aren't going to have a function where it was an untyped data pack.
            for (uint32_t i = 1; i < funcCode->childrenCount; i++)
            {
                auto c = &funcCode->children[i];
                assert(strcmp(c->metadata.str, "statement") == 0 || strcmp(c->metadata.str, "statement_noend") == 0 );
                GenerateStatement(c, fileWriter, 1, tableElem.key);
            }

            // TODO: maybe in a future version we do this a little differently. right now we check if
            // we are emit for the `main` func, and if so, we always make the last instruction ppl_exit().
            // otherwise, we do a `ret` instruction.

            if ( strcmp(tableElem.key, "main") == 0 )
            {
                // TODO: emit the register restore.
                auto s = SillyStringFmt(
                                        FUNC_END_LABEL_FMT ":\n"
                                        "call ppl_exit(r2_32)\n", tableElem.key);
                fileWriter.write(s);
            }
            else
            {
                // TODO: emit the register restore.
                auto s = SillyStringFmt(
                                        FUNC_END_LABEL_FMT ":\n"
                                        "ret\n", tableElem.key);
                fileWriter.write(s);
            }
        }
    }
    
    fileWriter.write(".section data\n");
    
    // generate the compile-time variable.
    for ( auto it = CG_Glob()->binaryReadonly.begin(); it != CG_Glob()->binaryReadonly.end(); it++ )
    {
        PPL_HashMapWithStringKey_Element<CG_Value> tableElem = *it;
        
        auto s = tableElem.key;

        switch(tableElem.value.valueKind)
        {
            PPL_TYPE_INTEGER_CASE
            {
                fileWriter.write("\nlabel_");
                fileWriter.write((char*)s);
                fileWriter.write(":\n");

                // TODO: since the write func does not modify the string that it takes
                // in, we can make it take a const char *.
                fileWriter.write(".db ");

                // NOTE: the value extract is gonna work between any integer type,
                // so long as we have the correct bit width.
                uint64_t integerValue = ValueExtract_Uint64(tableElem.value);

                char *integerString = SillyStringFmt("%u", integerValue);

                fileWriter.write(integerString);
            } break;
            case PPL_TYPE_STRING:
            {
                
                fileWriter.write("\nlabel_");
                fileWriter.write((char*)s);
                fileWriter.write(":\n");

                CG_String string = ValueExtract_CgString(tableElem.value);

                bool handleEscape = false;
                for (uint32_t i = 0; i < string.len; i++)
                {
                    uint8_t c = string.backing[i];
                    
                    if (handleEscape)
                    {
                        // TODO: does the escaped char thing code exist anywhere else in this codebase?
                        c = CG_ConvertEscapedChar(c);
                        handleEscape = false;
                    } else if (c == '\\') {
                        handleEscape = true;
                        continue;
                    }
                    auto s = SillyStringFmt(".db %d\n", c);
                    fileWriter.write(s);
                }
                
                fileWriter.write(".db 0");
                
            }   break;
            case PPL_TYPE_FUNC:
                break; // we can ignore since handled in .section code
            default:
                PPL_TODO;
        }
    }
}

#endif

#undef CG_MAX_TUPLE_ELEMS
#undef FUNC_END_LABEL_FMT
#undef FUNC_END_LABEL_NO_LABEL_FMT
#undef PPL_TYPE_INTEGER_CASE
