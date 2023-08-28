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

struct CG_String
{
    char *backing;
    size_t len;
};

// TODO: pull this out to common stuff for this project.
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
        return str; // TODO: what if the string we were looking at was deallocated?
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
    };
    
    CG_Value() : valueKind(PPL_TYPE_UNKNOWN), v_CgFunction() {}
};

// TODO: write our own hash map stuff because we want to learn how the
// hashing algorithms work.
// there's also other parts of this codebase where we use the hash maps.
// use this struct there.
//
// we could also make this version a specialization on a more generic hash map.
//

template <typename T_ValueKind>
struct CG_HashMapWithStringKey_Element
{
    char *key;
    T_ValueKind value;
};

template <typename T_ValueKind>
struct CG_HashMapWithStringKey;

// iterator for hash map.
template <typename T_ValueKind>
class CG_HashMapWithStringKey_ListType {
public:
    CG_HashMapWithStringKey_Element<T_ValueKind> operator*() { return *this->elem; }

    bool operator!=(CG_HashMapWithStringKey_ListType &other) const { return (other.elem != this->elem); }

    // `this++`.
    CG_HashMapWithStringKey_ListType operator++(int);

    CG_HashMapWithStringKey_Element<T_ValueKind> *elem;
    CG_HashMapWithStringKey<T_ValueKind>         *parent;

    CG_HashMapWithStringKey_ListType(
        CG_HashMapWithStringKey<T_ValueKind> *parent, CG_HashMapWithStringKey_Element<T_ValueKind> *inval) :
        elem(inval), parent(parent)
    {}
};

// TODO: some of this hash map is untested. maybe we should verify that the endVal is good.
template <typename T_ValueKind>
struct CG_HashMapWithStringKey
{

    CG_HashMapWithStringKey_ListType<T_ValueKind> begin()
    {
        auto start = CG_HashMapWithStringKey_ListType<T_ValueKind> { this, table_internal ? &table_internal[0] : nullptr};

        while( start.elem && start.elem->key == nullptr ) start.elem += 1;
        
        return start;
    };
    
    CG_HashMapWithStringKey_ListType<T_ValueKind>& end()
    {
        return this->endVal;
    };
    
    // return TRUE if hash map contains item at key, FALSE otherwise.
    bool get(const char *key, T_ValueKind *dst)
    {
        size_t i = stbds_shgeti(table_internal, key);
        bool bResult = i != -1;
        if (bResult)
        {
            *dst = table_internal[i].value;
        }
        return bResult;
    }
    
    void put(const char *str, T_ValueKind value)
    {
        stbds_shput(table_internal, str, value);
        size_t end = stbds_shlen(table_internal);
        endVal = CG_HashMapWithStringKey_ListType<T_ValueKind> {this, &table_internal[end-1] + 1};
    }

    CG_HashMapWithStringKey_ListType<T_ValueKind> endVal = {this, nullptr};
    CG_HashMapWithStringKey_Element<T_ValueKind> *table_internal = nullptr;
    
    ~CG_HashMapWithStringKey()
    {
        stbds_shfree(table_internal);
    }
    
};

template <typename T_ValueKind>
CG_HashMapWithStringKey_ListType<T_ValueKind> CG_HashMapWithStringKey_ListType<T_ValueKind>::operator++(int)
{
    auto r = *this;
    
    this->elem += 1;
    
    while( this->elem->key == nullptr && *this != parent->endVal ) this->elem += 1;
    
    return r;
}

// =====================================================

struct CG_Globals
{
    CG_HashMapWithStringKey<CG_Value> metaVars;

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
    g->metaVars.~CG_HashMapWithStringKey();
    StretchyBufferFree(g->funcSignatureRegistryScratch);
    
    // TODO: it's kind of annoying that we have to define the init value twice for this member of Glob.
    g->labelUID = 0;
}

// =====================================================

// TODO: we could easily replace the ValueExtract idea with a switch statement.

uint64_t ValueExtract_Uint64(CG_Value val)
{
    return val.v_Uint64;
}

ppl_type ValueExtract_PplType(CG_Value val)
{
    // TODO:
    return val.v_PplType;
}

CG_Function ValueExtract_CgFunction(CG_Value val)
{
    return val.v_CgFunction;
}

CG_Span<CG_FunctionSignature> ValueExtract_CgFunctionSignature(CG_Value val)
{
    return val.v_CgFunctionSignature;
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
        // TODO:
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
        }
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

                // TODO: might make this async in the future?
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
#if 0
            else if ( child2->type == AST_STRING_LITERAL )
            {
                kind = PPL_TYPE_STRING;
                if (infoOut)  infoOut->TL = child2;
            }
#endif
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

    // TODO: handle object.
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
            
            // TODO: handle other cases. right now I'm being really lazy.
        default:
            break;
        
    }

    return val;
}

static void RecordAndGenerateCompileTimeVarDecl(struct tree_node *ast, PFileWriter &fileWriter)
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
    
    // NOTE: record the variable.
    CG_Glob()->metaVars.put( varName->metadata.str, val );
    
    // NOTE: emit a comment if the switch below doesn't emit the var, just
    // so that we don't get lots of empty space.
    fileWriter.write("; recording ");
    fileWriter.write((char*)varName->metadata.str);

    // TODO: implement other types.
    switch(type)
    {
        // TODO: proper handle the negative types.
        PPL_TYPE_INTEGER_CASE
        {
            fileWriter.write("\nlabel_");
            fileWriter.write((char*)varName->metadata.str);
            fileWriter.write(":\n");

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
    builder->elems[builder->elemCount++] = exp;
    assert(builder->elemCount <= CG_MAX_TUPLE_ELEMS);

    if (exp->childrenCount > 1)
    {
        assert(exp->children[0].type == AST_OP);
        CollectTupleFromExpression(&exp->children[1], builder);
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
    const char *indentationStr = "\t";
    
    // ast node is
    /*
     "[(import_expression)(data_pack)(span_expression)(assignment_exp)(conditional_exp)]",
     "((op,,)(expression))*"
     */
    
    // TODO: handle the Tuple expressions more generally. right now we only handle for function params.
    assert(ast->childrenCount >= 1);
    
    auto c = &ast->children[0];
    
    if (strcmp(c->metadata.str, "assignment_exp") == 0)
    {
        PPL_TODO;
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
             strcmp(c->metadata.str, "additive_exp") == 0 ||
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
        assert(callee->childrenCount >= 1);
        auto calleeName = callee->children[0].metadata.str;
        
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
        
        CG_Value val;
        if ( CG_Glob()->metaVars.get( c->metadata.str, &val) )
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
        else
        {
            // TODO: want a user error. I'm thinking something like CgEmitError;
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
    const char *indentationStr = "\t";
    
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
    else if (strcmp(n, "runtime_time_var_decl") == 0 )
    {
        PPL_TODO;
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
        PPL_TODO;
    }
    // TODO: handle the keywords of fall, break, continue.
    else if (strcmp(n, "if_statement") == 0 )
    {
        PPL_TODO;
    }
    else if (strcmp(n, "while_statement") == 0 )
    {
        PPL_TODO;
    }
    else if (strcmp(n, "for_statement") == 0 )
    {
        PPL_TODO;        
    }
    else if (strcmp(n, "switch_statement") == 0 )
    {
        PPL_TODO;        
    }
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
    
    fileWriter.write(".section data\n");

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
                RecordAndGenerateCompileTimeVarDecl(&child, fileWriter);
                fileWriter.write("\n");
            }
        }
    }
    
    fileWriter.write("\n.section code\n");
    
    // TODO: need to impl additional operators on the iterator thing.
    for ( auto it = CG_Glob()->metaVars.begin(); it != CG_Glob()->metaVars.end(); it++)
    {
        CG_HashMapWithStringKey_Element<CG_Value> tableElem = *it;

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
                                        "\tcall ppl_exit(r2)\n", tableElem.key);
                fileWriter.write(s);
            }
            else
            {
                // TODO: emit the register restore.
                auto s = SillyStringFmt(
                                        FUNC_END_LABEL_FMT ":\n"
                                        "\tret\n", tableElem.key);
                fileWriter.write(s);
            }
        }
    }
}

#endif

#undef CG_MAX_TUPLE_ELEMS
#undef FUNC_END_LABEL_FMT
#undef FUNC_END_LABEL_NO_LABEL_FMT
#undef PPL_TYPE_INTEGER_CASE
