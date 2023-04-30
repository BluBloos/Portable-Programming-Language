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
        "Any", "Type", "TypeInfo", "TypeInfoMember"
    };

    

}

enum ppl_type {
    PPL_TYPE_UNKNOWN,
    PPL_TYPE_F32,
    PPL_TYPE_F64,
    PPL_TYPE_FLOAT = PPL_TYPE_F32,
    PPL_TYPE_DOUBLE = PPL_TYPE_F64,
    PPL_TYPE_BOOL,
    PPL_TYPE_VOID,
    PPL_TYPE_U8, PPL_TYPE_U16, PPL_TYPE_U32, PPL_TYPE_U64,
    PPL_TYPE_S8, PPL_TYPE_S16, PPL_TYPE_S32, PPL_TYPE_S64,
    PPL_TYPE_INT = PPL_TYPE_S64,
    PPL_TYPE_CHAR = PPL_TYPE_S8,
    PPL_TYPE_SHORT = PPL_TYPE_S16,
    PPL_TYPE_ANY,
    PPL_TYPE_TYPE,
    PPL_TYPE_TYPE_INFO,
    PPL_TYPE_TYPE_INFO_MEMBER
};

#endif // include guard.
 