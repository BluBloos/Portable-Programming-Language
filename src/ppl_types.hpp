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
        "struct", "enum", "enum_flag"
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
    PPL_TYPE_TYPE_INFO_MEMBER,
    PPL_TYPE_STRUCT,
    PPL_TYPE_ENUM,
    PPL_TYPE_ENUM_FLAG
};

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
        case PPL_TYPE_STRUCT:
            return "PPL_TYPE_STRUCT";
        case PPL_TYPE_ENUM:
            return "PPL_TYPE_ENUM";
        case PPL_TYPE_ENUM_FLAG:
            return "PPL_TYPE_ENUM_FLAG";
    }
}

#endif // include guard.
 