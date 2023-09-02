#if !defined(PPL_TYPES_HPP)
#define PPL_TYPES_HPP

namespace ppl {

// TODO: would be nice to clean some of this stuff up

    char *TYPE_STRINGS[] = {

        // floating point types.
        "float", "double",
        "f32", "f64",

        "bool", "void",

        // integer types.
        "u8", "u16", "u32", "u64",
        "s8", "s16", "s32", "s64",

        // alias types that we carry over from C because we are nice.
        "int", "char", "short",

        // cool types.
        "Any", "Type", "TypeInfo", "TypeInfoMember",

        // other cool types.
        "struct", "enum", "enum_flag",
        
        // TODO: maybe just shorten this to "space".
        "namespace"
    };

    

}

enum ppl_type {
    PPL_TYPE_UNKNOWN,
    PPL_TYPE_F32,
    PPL_TYPE_F64,
    PPL_TYPE_FLOAT  = PPL_TYPE_F32,
    PPL_TYPE_DOUBLE = PPL_TYPE_F64,
    PPL_TYPE_BOOL,
    PPL_TYPE_VOID,
    PPL_TYPE_U8, PPL_TYPE_U16, PPL_TYPE_U32, PPL_TYPE_U64,
    PPL_TYPE_S8, 
    PPL_TYPE_CHAR = PPL_TYPE_S8,
    PPL_TYPE_S16, 
    PPL_TYPE_SHORT = PPL_TYPE_S16,
    PPL_TYPE_S32,
    PPL_TYPE_S64,
    PPL_TYPE_INT = PPL_TYPE_S64,
    PPL_TYPE_ANY,
    PPL_TYPE_TYPE,
    PPL_TYPE_TYPE_INFO,
    PPL_TYPE_STRUCT = PPL_TYPE_TYPE_INFO,
    PPL_TYPE_TYPE_INFO_MEMBER,
    PPL_TYPE_ENUM,
    PPL_TYPE_ENUM_FLAG,
    PPL_TYPE_NAMESPACE,
    PPL_TYPE_SPAN,
    PPL_TYPE_FUNC,
    PPL_TYPE_FUNC_SIGNATURE,

    // array types. recall that array types are just views of some base array.
    // the base array is just a view too, but it's a view of the whole thing.
    PPL_TYPE_CHAR_ARRAY,
    PPL_TYPE_STRING = PPL_TYPE_CHAR_ARRAY
};

int PplTypeGetWidth(ppl_type type)
{
    switch(type)
    {
        case PPL_TYPE_BOOL:
        case PPL_TYPE_U8:
        case PPL_TYPE_S8:
            return 1;

        case PPL_TYPE_S16:
        case PPL_TYPE_U16:
            return 2;

        case PPL_TYPE_F32:
        case PPL_TYPE_U32:
        case PPL_TYPE_S32:
            return 4;

        case PPL_TYPE_U64:
        case PPL_TYPE_F64:
        case PPL_TYPE_S64:
            return 8;
            
        default:
            PPL_TODO;
            return 0;
    }
}

// NOTE: this returns true if the type is signed.
bool PplTypeGetSign(ppl_type type)
{
    switch(type)
    {
        case PPL_TYPE_BOOL:
        case PPL_TYPE_U16:
        case PPL_TYPE_U8:
        case PPL_TYPE_U32:
        case PPL_TYPE_U64:
            return false;
        
        case PPL_TYPE_S8:
        case PPL_TYPE_S16:
        case PPL_TYPE_F32:
        case PPL_TYPE_S32:
        case PPL_TYPE_F64:
        case PPL_TYPE_S64:
            return true;
            
        default:
            PPL_TODO;
            return false;
    }
}

static ppl_type KeywordToPplType(const char *str)
{
    assert (str);
    char fc = str[0];
    switch(fc)
    {
        case 'f':
        {
            if (strcmp(str, "float") == 0 || strcmp(str, "f32") == 0)
            {
                return PPL_TYPE_FLOAT;
            }
            return PPL_TYPE_DOUBLE;
        } break;
        case 'd':
        return PPL_TYPE_DOUBLE;
        case 'b':
        return PPL_TYPE_BOOL;
        case 'v':
        return PPL_TYPE_VOID;
        case 'u':
        {
            char sc = str[1];
            switch(sc)
            {
                case '8': // u8
                return PPL_TYPE_U8;
                case '1': // u16
                return PPL_TYPE_U16;
                case '3': // u32
                return PPL_TYPE_U32;
            }
            return PPL_TYPE_U64;
        } break;
        case 's':
        {
            char sc = str[1];
            switch(sc)
            {
                case '8': // s8
                return PPL_TYPE_S8;
                case '1': // s16
                case 'h': // short
                return PPL_TYPE_S16;
                case '3': // s32
                return PPL_TYPE_S32;
            }
            return PPL_TYPE_S64;
        } break;
        case 'i':
        return PPL_TYPE_S64;
        case 'c':
        return PPL_TYPE_CHAR;
        case 'A':
        return PPL_TYPE_ANY;
        case 'T':
        {
            if (strcmp(str, "TypeInfo") == 0)
                return PPL_TYPE_TYPE_INFO;
            if (strcmp(str, "TypeInfoMember") == 0)
                return PPL_TYPE_TYPE_INFO_MEMBER;
            return PPL_TYPE_TYPE;
        } break;
        case 'e':
        {
            if (strcmp(str, "enum_flag") == 0)
                return PPL_TYPE_ENUM_FLAG;
            return PPL_TYPE_ENUM;
        } break;
        case 'n':
        return PPL_TYPE_NAMESPACE;
    }
    return PPL_TYPE_UNKNOWN;
}

static const char * PplTypeToString(ppl_type type)
{
    switch (type) {
        case PPL_TYPE_UNKNOWN:
            return "PPL_TYPE_UNKNOWN";
        case PPL_TYPE_F32:
            return "PPL_TYPE_F32";
        case PPL_TYPE_F64:
            return "PPL_TYPE_F64";
        case PPL_TYPE_BOOL:
            return "PPL_TYPE_BOOL";
        case PPL_TYPE_VOID:
            return "PPL_TYPE_VOID";
        case PPL_TYPE_U8:
            return "PPL_TYPE_U8";
        case PPL_TYPE_U16:
            return "PPL_TYPE_U16";
        case PPL_TYPE_U32:
            return "PPL_TYPE_U32";
        case PPL_TYPE_U64:
            return "PPL_TYPE_U64";
        case PPL_TYPE_S8:
            return "PPL_TYPE_S8";
        case PPL_TYPE_S16:
            return "PPL_TYPE_S16";
        case PPL_TYPE_S32:
            return "PPL_TYPE_S32";
        case PPL_TYPE_S64:
            return "PPL_TYPE_S64";
        case PPL_TYPE_ANY:
            return "PPL_TYPE_ANY";
        case PPL_TYPE_TYPE:
            return "PPL_TYPE_TYPE";
        case PPL_TYPE_TYPE_INFO:
            return "PPL_TYPE_TYPE_INFO";
        case PPL_TYPE_TYPE_INFO_MEMBER:
            return "PPL_TYPE_TYPE_INFO_MEMBER";
        case PPL_TYPE_ENUM:
            return "PPL_TYPE_ENUM";
        case PPL_TYPE_ENUM_FLAG:
            return "PPL_TYPE_ENUM_FLAG";
        case PPL_TYPE_NAMESPACE:
            return "PPL_TYPE_NAMESPACE";
        case PPL_TYPE_SPAN:
            return "PPL_TYPE_SPAN";
        case PPL_TYPE_FUNC:
            return "PPL_TYPE_FUNC";
        case PPL_TYPE_FUNC_SIGNATURE:
            return "PPL_TYPE_FUNC_SIGNATURE";
        case PPL_TYPE_CHAR_ARRAY:
            return "PPL_TYPE_CHAR_ARRAY";
    }
}

const char *PplTypeToPasmHumanReadable(ppl_type type)
{
    switch (type) {
        case PPL_TYPE_F32:
            return "float32";
        case PPL_TYPE_F64:
            return "float64";
        case PPL_TYPE_U8:
        case PPL_TYPE_BOOL:
            return "uint8";
        case PPL_TYPE_U16:
            return "uint16";
        case PPL_TYPE_U32:
            return "uint32";
        case PPL_TYPE_U64:
            return "uint64";
        case PPL_TYPE_S8:
            return "int8";
        case PPL_TYPE_S16:
            return "int16";
        case PPL_TYPE_S32:
            return "int32";
        case PPL_TYPE_S64:
            return "int64";
        case PPL_TYPE_VOID:
            return "void";
        
        // TODO: this is gonna force a PASM failure. prob want to do something more
        // intelligent here.
        default:
            PPL_TODO;
            return "<unknown>";
    }
}

#endif // include guard.
 