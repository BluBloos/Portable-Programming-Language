// This file takes the human readable text representation of the PPL IR
// and converts it into an in-memory representation suitable for translation into
// arbitrary target platforms such as x86_64, x86, 64 bit ARM, webassmely, and
// generally, and RISC / CISC based architecture.

/* TODO(Noah): 
    Implement variadic types for .def as well.
    Implement the parsing of negative integers (right now it seems that they are 
    intepreted as labels. And of course this works because NASM knows what do with 
    negative numbers in the source file).
*/

#ifndef PASSEMBLER_H
#define PASSEMBLER_H

enum pasm_line_type {
    PASM_LINE_UNDEFINED = 0,
    PASM_LINE_SECTION_CODE,
    PASM_LINE_SECTION_DATA,
    PASM_LINE_LABEL,
    PASM_LINE_FDECL,
    PASM_LINE_DATA_BYTE_INT,
    PASM_LINE_DATA_BYTE_STRING,
    PASM_LINE_FCALL,
    PASM_LINE_FDEF,
    PASM_LINE_SAVE,
    PASM_LINE_RESTORE,
    PASM_LINE_BRANCH,
    PASM_LINE_BRANCH_GT,
    PASM_LINE_RET,
    // TODO(Noah): For instructions such as MOV, SUB, etc, 
    // we might want to do some semantic checking. Because as of
    // right now, our parsing supports for invalid assembly grammers.
    PASM_LINE_MOV,
    PASM_LINE_SUB,
    PASM_LINE_ADD,
    PASM_LINE_XOR
};

// calling convention
enum pasm_cc {
    PASM_CC_PDECL = 0,
    PASM_CC_CDECL
};

// TODO(Noah): I'm pretty sure it's feasible to have 128-bit floats
// Also, what about the SIMD registers (if they even are registers)?
enum pasm_type {
    PASM_VOID = 0,
    PASM_INT8,
    PASM_INT16,
    PASM_INT32,
    PASM_INT64,
    PASM_UINT8,
    PASM_UINT16,
    PASM_UINT32,
    PASM_UINT64,
    PASM_FLOAT32,
    PASM_FLOAT64,
    PASM_INT64_VARIADIC
};

// Function declaration.
struct pasm_fdecl {
    enum pasm_cc callingConvention;
    enum pasm_type returnType;
    char *name;
    enum pasm_type *params; // Stretchy buffer.
};

enum pasm_fparam_type {
    PASM_FPARAM_LABEL = 0,
    PASM_FPARAM_INT,
    PASM_FPARAM_REGISTER,
    PASM_FPARAM_STACKVAR
};

enum pasm_register {
    PASM_R0 = 0, PASM_R1, PASM_R2, PASM_R3,
    PASM_R4, PASM_R5, PASM_R6, PASM_R7,
    PASM_R8, PASM_R9, PASM_R10, PASM_R11,
    PASM_R12, PASM_R13, PASM_R14, PASM_R15,
    PASM_R16, PASM_R17, PASM_R18, PASM_R19,
    PASM_R20, PASM_R21, PASM_R22, PASM_R23,
    PASM_R24, PASM_R25, PASM_R26, PASM_R27,
    PASM_R28, PASM_R29, PASM_R30, PASM_R31,

    PASM_R0_32, PASM_R1_32, PASM_R2_32, PASM_R3_32,
    PASM_R4_32, PASM_R5_32, PASM_R6_32, PASM_R7_32,
    PASM_R8_32, PASM_R9_32, PASM_R10_32, PASM_R11_32,
    PASM_R12_32, PASM_R13_32, PASM_R14_32, PASM_R15_32,
    PASM_R16_32, PASM_R17_32, PASM_R18_32, PASM_R19_32,
    PASM_R20_32, PASM_R21_32, PASM_R22_32, PASM_R23_32,
    PASM_R24_32, PASM_R25_32, PASM_R26_32, PASM_R27_32,
    PASM_R28_32, PASM_R29_32, PASM_R30_32, PASM_R31_32,

    PASM_R0_16, PASM_R1_16, PASM_R2_16, PASM_R3_16,
    PASM_R4_16, PASM_R5_16, PASM_R6_16, PASM_R7_16,
    PASM_R8_16, PASM_R9_16, PASM_R10_16, PASM_R11_16,
    PASM_R12_16, PASM_R13_16, PASM_R14_16, PASM_R15_16,
    PASM_R16_16, PASM_R17_16, PASM_R18_16, PASM_R19_16,
    PASM_R20_16, PASM_R21_16, PASM_R22_16, PASM_R23_16,
    PASM_R24_16, PASM_R25_16, PASM_R26_16, PASM_R27_16,
    PASM_R28_16, PASM_R29_16, PASM_R30_16, PASM_R31_16,

    PASM_R0_8, PASM_R1_8, PASM_R2_8, PASM_R3_8,
    PASM_R4_8, PASM_R5_8, PASM_R6_8, PASM_R7_8,
    PASM_R8_8, PASM_R9_8, PASM_R10_8, PASM_R11_8,
    PASM_R12_8, PASM_R13_8, PASM_R14_8, PASM_R15_8,
    PASM_R16_8, PASM_R17_8, PASM_R18_8, PASM_R19_8,
    PASM_R20_8, PASM_R21_8, PASM_R22_8, PASM_R23_8,
    PASM_R24_8, PASM_R25_8, PASM_R26_8, PASM_R27_8,
    PASM_R28_8, PASM_R29_8, PASM_R30_8, PASM_R31_8,
};

struct pasm_stackvar {
    int addr;
    char *name;
    enum pasm_type type;
};

// Function params
struct pasm_fparam {
    enum pasm_fparam_type type;
    union {
        struct pasm_stackvar data_sv;
        char *data_cptr;
        int data_int;
        enum pasm_register data_register;
    };
};

// Function call.
struct pasm_fcall {
    char *name;
    struct pasm_fparam *params; // Stretchy buffer.
};

struct pasm_fnparam {
    enum pasm_type type;
    char *name;
};

struct pasm_fdef {
    char *name;
    enum pasm_type returnType;
    struct pasm_fnparam *params; // Stretchy buffer.
};

struct pasm_fptriad {
    struct pasm_fparam param1;
    struct pasm_fparam param2;
    char *param3;
};

struct pasm_line {
    enum pasm_line_type lineType;
    union {
        char *data_cptr;
        struct pasm_fdecl data_fdecl;
        struct pasm_fcall data_fcall;
        struct pasm_fdef data_fdef;
        enum pasm_register *data_save; // Stretchy buffer.
        int data_int;
        struct pasm_fptriad data_fptriad;
    };
};

struct pasm_fcall PasmFcallEmpty() {
    struct pasm_fcall pfcall;
    pfcall.name = NULL;
    pfcall.params = NULL;
    return pfcall;
}

struct pasm_fdecl PasmFdeclEmpty() {
    struct pasm_fdecl pfdecl;
    pfdecl.callingConvention = PASM_CC_PDECL;
    pfdecl.returnType = PASM_VOID;
    pfdecl.name = NULL;
    pfdecl.params = NULL;
    return pfdecl;
}

struct pasm_line PasmLineEmpty() {
    struct pasm_line pl;
    pl.lineType = PASM_LINE_UNDEFINED;
    pl.data_cptr = (char *)0;
    pl.data_fdecl = PasmFdeclEmpty();
    pl.data_fcall = PasmFcallEmpty();
    pl.data_int = 0;
    return pl;
}

struct pasm_fdecl_table {
    char *key;
    struct pasm_fdecl value;
};

struct pasm_fdef_table {
    char *key;
    struct pasm_fdef value;
};

struct pasm_label_table {
    char *key;
    int value;
};

// TODO(Noah): See. Crap like this, in PPL, can be made MUCH more clean.
// This type of code is prone to error big time.
void PasmTypePrint(enum pasm_type ptype) {
    switch(ptype) {
        case PASM_VOID:
        LOGGER.Min("PASM_VOID\n");
        break;
        case PASM_INT8:
        LOGGER.Min("PASM_INT8\n");
        break;
        case PASM_INT16:
        LOGGER.Min("PASM_INT16\n");
        break;
        case PASM_INT32:
        LOGGER.Min("PASM_INT32\n");
        break;
        case PASM_INT64:
        LOGGER.Min("PASM_INT64\n");
        break;
        case PASM_UINT8:
        LOGGER.Min("PASM_UINT8\n");
        break;
        case PASM_UINT16:
        LOGGER.Min("PASM_UINT16\n");
        break;
        case PASM_UINT32:
        LOGGER.Min("PASM_UINT32\n");
        break;
        case PASM_UINT64:
        LOGGER.Min("PASM_UINT64\n");
        break;
        case PASM_FLOAT32:
        LOGGER.Min("PASM_FLOAT32\n");
        break;
        case PASM_FLOAT64:
        LOGGER.Min("PASM_FLOAT64\n");
        break;
        case PASM_INT64_VARIADIC:
        LOGGER.Min("PASM_INT64_VARIADIC\n");
        break;
    }
}

void PasmFparamPrint(struct pasm_fparam fparam) {
    switch(fparam.type) {
        case PASM_FPARAM_INT:
        LOGGER.Min("  PASM_FPARAM_INT\n");
        LOGGER.Min("    %d\n", fparam.data_int);
        break;
        case PASM_FPARAM_LABEL:
        LOGGER.Min("  PASM_FPARAM_LABEL\n");
        LOGGER.Min("    %s\n", fparam.data_cptr);
        break;
        case PASM_FPARAM_REGISTER:
        LOGGER.Min("  PASM_FPARAM_REGISTER\n");
        {
            unsigned int reg = (int)fparam.data_register % 32;
            unsigned int bitness = ((int)fparam.data_register / 32);
            LOGGER.Min("    r%d_%d\n", reg, bitness);
        }
        break;
        case PASM_FPARAM_STACKVAR:
        LOGGER.Min("  PASM_FPARAM_STACKVAR\n");
        LOGGER.Min("    name:%s\n", fparam.data_sv.name);
        LOGGER.Min("    addr:%d\n", fparam.data_sv.addr);
        break;
    }
}

void PasmCCPrint(enum pasm_cc cc) {
    switch(cc) {
        case PASM_CC_PDECL:
        LOGGER.Min("PASM_CC_PDECL\n");
        break;
        case PASM_CC_CDECL:
        LOGGER.Min("PASM_CC_CDECL\n");
        break;
    }
}

void PasmLinePrint(struct pasm_line pl) {
    switch(pl.lineType) {
        case PASM_LINE_UNDEFINED:
        LOGGER.Min("PASM_LINE_UNDEFINED\n");
        break;
        case PASM_LINE_RET:
        LOGGER.Min("PASM_LINE_RET\n");
        break;
        case PASM_LINE_SECTION_CODE:
        LOGGER.Min("PASM_LINE_SECTION_CODE\n");
        break;
        case PASM_LINE_SECTION_DATA:
        LOGGER.Min("PASM_LINE_SECTION_DATA\n");
        break;
        case PASM_LINE_LABEL:
        LOGGER.Min("PASM_LINE_LABEL\n");
        LOGGER.Min("  %s\n", pl.data_cptr);
        break;
        case PASM_LINE_BRANCH:
        LOGGER.Min("PASM_LINE_BRANCH\n");
        LOGGER.Min("  %s\n", pl.data_cptr);
        break;
        case PASM_LINE_RESTORE: 
        {
            LOGGER.Min("PASM_LINE_RESTORE\n");
            for (int i = 0; i < StretchyBufferCount(pl.data_save); i++) {
                LOGGER.Min("  r%d\n", (int)pl.data_save[i]);
            }
        }
        break;
        case PASM_LINE_SAVE: 
        {
            LOGGER.Min("PASM_LINE_SAVE\n");
            for (int i = 0; i < StretchyBufferCount(pl.data_save); i++) {
                LOGGER.Min("  r%d\n", (int)pl.data_save[i]);
            }
        }
        break;
        case PASM_LINE_FDECL:
        {
            LOGGER.Min("PASM_LINE_FDECL\n");
            LOGGER.Min("  name:%s\n", pl.data_fdecl.name);
            LOGGER.Min("  callingConvention:"); PasmCCPrint(pl.data_fdecl.callingConvention);
            LOGGER.Min("  returnType:"); PasmTypePrint(pl.data_fdecl.returnType);
            LOGGER.Min("  params:\n");
            for (int i = 0; i < StretchyBufferCount(pl.data_fdecl.params); i++) {
                LOGGER.Min("    "); PasmTypePrint(pl.data_fdecl.params[i]);
            }
        }
        break;
        case PASM_LINE_FDEF:
        {
            LOGGER.Min("PASM_LINE_FDEF\n");
            LOGGER.Min("  name:%s\n", pl.data_fdef.name);
            LOGGER.Min("  returnType:"); PasmTypePrint(pl.data_fdef.returnType);
            LOGGER.Min("  params:\n");
            for (int i = 0; i < StretchyBufferCount(pl.data_fdef.params); i++) {
                struct pasm_fnparam fnparam = pl.data_fdef.params[i];
                LOGGER.Min("    type:"); PasmTypePrint(fnparam.type);
                LOGGER.Min("    name:%s\n", fnparam.name);
            }
        }
        break;
        case PASM_LINE_DATA_BYTE_STRING:
        LOGGER.Min("PASM_LINE_DATA_BYTE_STRING\n");
        LOGGER.Min("  %s\n", pl.data_cptr);
        break;
        case PASM_LINE_DATA_BYTE_INT:
        LOGGER.Min("PASM_LINE_DATA_BYTE_INT\n");
        LOGGER.Min("  %d\n", pl.data_int);
        break;
        case PASM_LINE_MOV:
        case PASM_LINE_SUB:
        case PASM_LINE_ADD:
        case PASM_LINE_XOR:
        {
            switch(pl.lineType) {
                case PASM_LINE_MOV:
                LOGGER.Min("PASM_LINE_MOV\n");
                break;
                case PASM_LINE_SUB:
                LOGGER.Min("PASM_LINE_SUB\n");
                break;
                case PASM_LINE_ADD:
                LOGGER.Min("PASM_LINE_ADD\n");
                break;
                case PASM_LINE_XOR:
                LOGGER.Min("PASM_LINE_XOR\n");
                break;
                default:
                break;
            }
            PasmFparamPrint(pl.data_fptriad.param1);
            PasmFparamPrint(pl.data_fptriad.param2);
        }
        break;
        case PASM_LINE_BRANCH_GT:
        LOGGER.Min("PASM_LINE_BRANCH_GT\n");
        PasmFparamPrint(pl.data_fptriad.param1);
        PasmFparamPrint(pl.data_fptriad.param2);
        LOGGER.Min("  param3:%s\n", pl.data_fptriad.param3);
        break;
        case PASM_LINE_FCALL:
        {
            LOGGER.Min("PASM_LINE_FCALL\n");
            LOGGER.Min("  name:%s\n", pl.data_fcall.name);
            LOGGER.Min("  params:\n");
            for (int i = 0; i < StretchyBufferCount(pl.data_fcall.params); i++) {
                struct pasm_fparam fparam = pl.data_fcall.params[i];
                PasmFparamPrint(fparam);
            }
        }
        break;
    }
}

// NOTE(Noah): I could not dislike this function more.
// Anything else please. Be it regex, maps, whatever. You name it.
// But this? This is like your siblings wet fart on a thursday morning.
//
// return true if str is equal to any one of the 32 general purpose
// registers. Ex) "r20", "r31"...
// If the function returns true, reg will be set to the corresponding
// register.
bool SillyStringGetRegister(char *str, enum pasm_register &reg) {
    if (str[0] != 'r') {
        return false;
    }
    str++; // skip past the 'r'.
    std::string regNum = "";
    char *pStr = str;
    for (; *pStr != 0 && *pStr != '_'; pStr++) {
        regNum += *pStr;
    }
    unsigned int num = SillyStringToUINT((char *)regNum.c_str());    
    if (num >= 0 && num < 32) {
        reg = (enum pasm_register)((int)PASM_R0 + num);
        if (*pStr == 0) {
            // Good to go. The register is 64 bit.
            return true;
        } else {
            pStr++; // Skip past the underscore.
            unsigned int num2 = SillyStringToUINT(pStr);
            unsigned int offset = 3 - log2(num2 / 8);
            reg = (enum pasm_register)((unsigned int)reg + offset * 32);
            return true;
        }
    }

    return false;
}

void HandleLine(char *line);
struct pasm_line *pasm_lines = NULL; // stretchy buffer.

struct pasm_fdecl_table *fdecl_table = NULL; // empty stb hashmap.
struct pasm_fdef_table *fdef_table = NULL; // empty stb hashmap.
struct pasm_label_table *label_table = NULL; // empty stb hashmap.

// For defining stack variables.
// strings are not managed by the this table. Just a ref to an already existing string.
struct pasm_stackvar *_sv_table = NULL; // stretchy buffer.

// Deallocs all resources allocated by pasm_main
void DeallocPasm() {
    struct pasm_line *lines = pasm_lines;
    for (int i = 0; i < StretchyBufferCount(lines); i++) {
        struct pasm_line pline = lines[i];
        switch(pline.lineType) {
            case PASM_LINE_FDECL:
            StretchyBufferFree(pline.data_fdecl.params);
            break;
            case PASM_LINE_FCALL:
            StretchyBufferFree(pline.data_fcall.params);
            break;
            case PASM_LINE_FDEF:
            StretchyBufferFree(pline.data_fdef.params);
            break;
            case PASM_LINE_SAVE:
            case PASM_LINE_RESTORE:
            StretchyBufferFree(pline.data_save);
            default:
            break;
        }
    }
    StretchyBufferFree(lines);
    // NOTE(Noah): Read this as "stb datastructures hashmap free".
    stbds_shfree(fdecl_table);
    stbds_shfree(fdef_table);
    stbds_shfree(label_table);
}

/* Given a fparam that is of type PASM_FPARAM_LABEL,
   check if this label corresponds with a stack variable.
   If it does, adjust the fparam to be PASM_FPARAM_STACKVAR
   and set data_sv accordingly. */
void StackVarFromFplabel(struct pasm_fparam *fparam) {
    // TODO(Noah): check if the label is in the label table.
    //
    // we start by going right to see if this is a local var.
    bool exists = false;
    struct pasm_stackvar sv;
    int j = StretchyBufferCount(_sv_table) - 1;
    while (StretchyBufferCount(_sv_table) && 
        !SillyStringStartsWith(_sv_table[j].name, 
            "0scope")) 
    {
        if (SillyStringEquals(fparam->data_cptr, 
            _sv_table[j].name)) 
        {
            exists = true;
            sv = _sv_table[j];    
            break;
        }   
        j--;
    }
    if (exists) {
        fparam->type = PASM_FPARAM_STACKVAR;
        fparam->data_sv = sv;
    }

}

// USAGE:
// pplasm <inFile> <targetPlatform> 
int pasm_main(int argc, char **argv) {

    // StretchyBufferInit(pasm_lines);
    pasm_lines = NULL;
    _sv_table = NULL;

    if (argc < 3) {
        LOGGER.Error("Not enough arguments");
        return 1;
    } else {

        char *inFilePath = argv[1];
        char *targetPlatform = argv[2];

        FILE *inFile = fopen(inFilePath, "r");

        if (inFile != NULL) {
            // To make life easy, we define that all assembly lines will be written on exactly one
            // line in the source file. The text will be ASCII encoded. There won't even be escapes to
            // allow for multi-liners.

            char *p = NULL; size_t lineCap;
            ssize_t r = getline(&p, &lineCap, inFile);

            // TODO(Noah): This code is probably going to be really slow because for every line of
            // the assembler file that we take in, we fucking allocate and dealloc memory.
            // it would be much more sensible to just make a buffer of size 1024 or smthn and just put the
            // ASCII formated lines into there.
            // and we check if the line is larger, if and only if the line is larger, then we allocate more memory.
            // but for most lines, 1024 should be good.
            while ( r != -1 ) {
                HandleLine(p);
                free(p);
                p = NULL;
                r = getline(&p, &lineCap, inFile);
            }

            // even if r is -1 (which means we reached the end of the file),
            // it could be the case that there is like one last line to handle?
            if (p != NULL) {
                HandleLine(p);
                free(p);
                p = NULL;
            }

            fclose(inFile);
        } else {
            LOGGER.Error("Unable to open inFile=%s", inFilePath);
            return 1;
        }

    }

    if (VERBOSE) {
        // say that it is pass #1
        LOGGER.Min(ColorHighlight);
        LOGGER.Min("pass #1\n");
        LOGGER.Min(ColorNormal);  
        for (int i = 0; i < StretchyBufferCount(pasm_lines); i++) {
            PasmLinePrint(pasm_lines[i]);
        }
    }

    // go thru all the lines again for a second pass, resolving labels -> stack vars
    // Need to do in another pass because global labels take precedence over local
    // vars.
    for (int i = 0; i < StretchyBufferCount(pasm_lines); i++) {
        // TODO(Noah): Implement the resolving of local vars
        // as stack variables.
        //
        // otherwise right now, we just care to implement function params.
        struct pasm_line pl = pasm_lines[i];
        switch(pl.lineType) {
            case PASM_LINE_FDEF:
            {
                // NOTE(Noah): this type of solution for scoping should work good for our
                // top-level function scopes and stuff. It should also be extensible 
                // for local vars and continuous lower-level scopes.
                //     
                // delete the current scope.
                while(StretchyBufferCount(_sv_table) && 
                    !SillyStringStartsWith(StretchyBufferLast(_sv_table).name, "0scope")) {
                    StretchyBufferPop(_sv_table);
                }
                
                // create a new scope by simply reusing the old "0scope" elem.
                // or if there is no prior scope, actually create the "0scope" elem.
                if (StretchyBufferCount(_sv_table) == 0) {
                    struct pasm_stackvar sv; sv.name = "0scope";
                    StretchyBufferPush(_sv_table, sv);
                }

                // now we go through the named params of FDEF and put them on the stack.
                int n = StretchyBufferCount(pl.data_fdef.params);
                for (int j = 0; j < n; j++) {
                    struct pasm_fnparam fnparam = pl.data_fdef.params[j];
                    struct pasm_stackvar sv;
                    sv.type = fnparam.type;
                    sv.name = fnparam.name;
                    // TODO(Noah): Implement other sizings than 64-bit.
                    sv.addr = 16 + 8 * (n - 1) - (j * 8);
                    StretchyBufferPush(_sv_table, sv); 
                }
            }
            break;
            case PASM_LINE_ADD:
            case PASM_LINE_SUB:
            case PASM_LINE_MOV:
            case PASM_LINE_XOR:
            case PASM_LINE_BRANCH_GT:
            {
                // All of these use the fptriad, so we can do this just fine.
                struct pasm_fptriad fptriad = pl.data_fptriad;
                if (fptriad.param1.type == PASM_FPARAM_LABEL) {
                    StackVarFromFplabel(&pasm_lines[i].data_fptriad.param1);
                }
                if (fptriad.param2.type == PASM_FPARAM_LABEL) {
                    StackVarFromFplabel(&pasm_lines[i].data_fptriad.param2);
                }
            }
            break;
            case PASM_LINE_FCALL:
            {
                for (int j = 0; j < StretchyBufferCount(pl.data_fcall.params); j++) {
                    struct pasm_fparam fparam = pl.data_fcall.params[j];
                    if (fparam.type == PASM_FPARAM_LABEL) {
                        StackVarFromFplabel(&pasm_lines[i].data_fcall.params[j]);
                    }
                }
            }
            break;
            default:
            break;
        }
        
    }

    if (VERBOSE) {
        // say that it is pass #1
        LOGGER.Min(ColorHighlight);
        LOGGER.Min("pass #2\n");
        LOGGER.Min(ColorNormal);  
        for (int i = 0; i < StretchyBufferCount(pasm_lines); i++) {
            PasmLinePrint(pasm_lines[i]);
        }
    }

    StretchyBufferFree(_sv_table);
    return 0;

}

enum pasm_type SillyStringGetPasmType(char *typeStr) {
    // TODO(Noah): The code below is better done with a map.
    if (SillyStringStartsWith(typeStr, "int8")) {
        return PASM_INT8;
    } else if (SillyStringStartsWith(typeStr, "int16")) {
        return PASM_INT16;
    } else if (SillyStringStartsWith(typeStr, "int32")) {
        return PASM_INT32;
    } else if (SillyStringStartsWith(typeStr, "int64")) {
        return PASM_INT64;
    } else if (SillyStringStartsWith(typeStr, "uint8")) {
        return PASM_UINT8;
    } else if (SillyStringStartsWith(typeStr, "uint16")) {
        return PASM_UINT16;
    } else if (SillyStringStartsWith(typeStr, "uint32")) {
        return PASM_UINT32;
    } else if (SillyStringStartsWith(typeStr, "uint64")) {
        return PASM_UINT64;
    } else if (SillyStringStartsWith(typeStr, "float32")) {
        return PASM_FLOAT32;
    } else if (SillyStringStartsWith(typeStr, "float64")) {
        return PASM_FLOAT64;
    } else if (SillyStringStartsWith(typeStr, "[]int64")) {
        return PASM_INT64_VARIADIC;
    }
    return PASM_VOID;
}

/* Takes in a pointer to a silly string that is assumed to start at
   a type. The type will be parsed and returned, with the
   underlying silly string being advanced. */
enum pasm_type HandleType(char **pline) {
    char *line = *pline;
    enum pasm_type ptype = PASM_VOID;
    std::string type = "";
    while (*line != ' ') {
        type += *line++;
    }
    ptype = SillyStringGetPasmType((char *)type.c_str());
    *pline = line;
    return ptype;
}

/* Takes in a pointer to a silly string that is assumed to start at some
   generic word. Also takes in a set of terminating characters. The function will
   parse the generic word (terminating at any of cTerm), and return the word as a
   SillyString. The underlying silly string will be advanced. */
char *HandleSillyStringUntil(char **pline, char *cTerm) {
    char *line = *pline;
    std::string name = "";
    while(!SillyStringCharIn(cTerm, *line)) name += *line++;
    *pline = line;
    return MEMORY_ARENA.StdStringAlloc(name);
}

/* Takes in a pointer to a silly string that is assumed to start at some
   generic word. Also takes in a set of terminating characters. The function will
   parse the generic word (terminating at any of cTerm), and return the word as a 
   StdString. The underlying silly string will be advanced. */
std::string HandleStdStringUntil(char **pline, char *cTerm) {
    char *line = *pline;
    std::string name = "";
    while(!SillyStringCharIn(cTerm, *line)) name += *line++;
    *pline = line;
    return name;
}

/* Generate a pasm_fparam from a silly string. */
struct pasm_fparam PasmFparamFromSillyString(char *ss) {
    struct pasm_fparam fparam;
    bool _dflag;
    enum pasm_register _reg;
    if (SillyStringGetRegister(ss, _reg)) {
        fparam.type = PASM_FPARAM_REGISTER;
        fparam.data_register = _reg;
    } else if (SillyStringIsNumber(ss, _dflag)) {
        fparam.type = PASM_FPARAM_INT;
        fparam.data_int = SillyStringToUINT(ss);
    } else {
        // We know it's a label.
        fparam.type = PASM_FPARAM_LABEL;
        fparam.data_cptr = MEMORY_ARENA.StringAlloc(ss);
    }
    return fparam;
}

void HandleLine(char *line) {

    if (*line == '.') { // Found a directive.
        
        line++; // skip over the '.'
        std::string directive = HandleStdStringUntil(&line, " ");
        line++; // skip over the ' ' 
        
        if (directive == "section") {
            
            pasm_line pline = PasmLineEmpty();
            pline.lineType = (*line == 'c') ? PASM_LINE_SECTION_CODE :
                PASM_LINE_SECTION_DATA;
            StretchyBufferPush(pasm_lines, pline);

        } else if (directive == "def") {

            pasm_line pline = PasmLineEmpty();
            pline.lineType = PASM_LINE_FDEF;
            // NOTE(Noah): calling convention is always pdecl.
            pline.data_fdef.returnType = HandleType(&line);
            line++; // Skip over ' '
            pline.data_fdef.name = HandleSillyStringUntil(&line, "(");
            line++; // Skip over '('

            // Now we have to check for the parameters.
            while(*line != ')') {
                while (*line == ',' || *line == ' ') { line++; }
                std::string _type = HandleStdStringUntil(&line, " ");
                enum pasm_type ptype = SillyStringGetPasmType(
                    (char *)_type.c_str());
                line++; // skip past ' '
                // Parse param name.
                std::string pname = HandleStdStringUntil(&line, ",)");
                struct pasm_fnparam fnparam;
                fnparam.name = MEMORY_ARENA.StdStringAlloc(pname);
                fnparam.type = ptype;
                StretchyBufferPush(pline.data_fdef.params, fnparam);               
            }
            StretchyBufferPush(pasm_lines, pline);
            stbds_shput(fdef_table, pline.data_fdef.name, pline.data_fdef);
            
        } else if (directive == "extern") {

            pasm_line pline = PasmLineEmpty();
            pline.lineType = PASM_LINE_FDECL;    
            // First thing to check is the calling convention.
            {
                enum pasm_cc ecc = (*line == 'p') ? PASM_CC_PDECL : 
                PASM_CC_CDECL;
                while (*line++ != ' '); // Skip over until whitespace.
                pline.data_fdecl.callingConvention = ecc;
            }
            pline.data_fdecl.returnType = HandleType(&line);
            line++; // Skip over ' '
            pline.data_fdecl.name = HandleSillyStringUntil(&line, "(");
            line++; // Skip over '('
            // Now we have to check for the parameters.
            while(*line != ')') {
                while (*line == ',' || *line == ' ') { line++; }
                std::string _type = HandleStdStringUntil(&line, ",)");
                enum pasm_type ptype = SillyStringGetPasmType(
                    (char *)_type.c_str());
                StretchyBufferPush(pline.data_fdecl.params, ptype);              
            }
            StretchyBufferPush(pasm_lines, pline);
            stbds_shput(fdecl_table, pline.data_fdecl.name, pline.data_fdecl);

        } else if (directive == "db") {

            pasm_line pline = PasmLineEmpty();
            // TODO(Noah): Add more types of byte literals.
            //   Ex) Add negative integers.
            if (*line == '"') {
                pline.lineType = PASM_LINE_DATA_BYTE_STRING;
                line++; // skip over the "
                std::string strLiteral = HandleStdStringUntil(&line, "\"");
                pline.data_cptr = MEMORY_ARENA.StdStringAlloc(strLiteral); 
            } else {
                pline.lineType = PASM_LINE_DATA_BYTE_INT;
                // An integer
                SillyStringRemove0xA(line);
                pline.data_int = SillyStringToUINT(line);
            }
            StretchyBufferPush(pasm_lines, pline);  
            
        }

    } else if (*line == ';') {

        // Do nothing!!! Yay :)

    } else if (SillyStringStartsWith(line, "label_")) {

        while (*line++ != '_');
        // line should now point to after the underscore.
        std::string label = HandleStdStringUntil(&line, ":");
        char *pstr = MEMORY_ARENA.StdStringAlloc(label);
        pasm_line pline = PasmLineEmpty();
        pline.lineType = PASM_LINE_LABEL;
        pline.data_cptr = pstr;
        StretchyBufferPush(pasm_lines, pline);
        stbds_shput(label_table, pline.data_cptr, 1);

    } else {
        // We can now make the assumption that we are dealing
        // with a full-blown assembly command. So it's got the
        // pneumonic and everything.
        if (SillyStringStartsWith(line, "call")) {

            pasm_line pline = PasmLineEmpty();
            pline.lineType = PASM_LINE_FCALL;
            while (*line++ != ' ');
            pline.data_fcall.name = HandleSillyStringUntil(&line, "(");
            line++; // skip past the '('
            // Now we have to check for the parameters.
            while(*line != ')') {
                while (*line == ',' || *line == ' ') { line++; }
                // Function params are either going to be an immediate
                //   which would be a number of a label.
                // OR, function params are going to be a register.
                std::string param = HandleStdStringUntil(&line, ",)");
                struct pasm_fparam fparam = PasmFparamFromSillyString(
                    (char *)param.c_str());
                StretchyBufferPush(pline.data_fcall.params, fparam);               
            }
            StretchyBufferPush(pasm_lines, pline);

        } else  if (
            SillyStringStartsWith(line, "save") ||
            SillyStringStartsWith(line, "restore")
        ) {

            pasm_line pline = PasmLineEmpty();
            pline.lineType = (*line == 's') ? PASM_LINE_SAVE : PASM_LINE_RESTORE;
            while (*line++ != ' '); // Skip over whitespace.
            line++; // Skip over '['
            while(*line != ']') {
                while (*line == ',' || *line == ' ') { line++; }
                std::string reg = HandleStdStringUntil(&line, ",]");
                enum pasm_register _reg;
                SillyStringGetRegister(
                    (char *)reg.c_str(), _reg);        
                StretchyBufferPush(pline.data_save, _reg);
            }
            StretchyBufferPush(pasm_lines, pline);

        } else if (SillyStringStartsWith(line, "br")) {

            pasm_line pline = PasmLineEmpty();
            pline.lineType = PASM_LINE_BRANCH;
            while (*line++ != ' '); // Skip over whitespace.
            SillyStringRemove0xA(line);
            pline.data_cptr = MEMORY_ARENA.StringAlloc(line);
            StretchyBufferPush(pasm_lines, pline);

        } else if (SillyStringStartsWith(line, "bgt")) {

            pasm_line pline = PasmLineEmpty();
            pline.lineType = PASM_LINE_BRANCH_GT;
            while (*line++ != ' '); // Skip over whitespace.
            std::string param1 = HandleStdStringUntil(&line, ",");
            struct pasm_fparam fparam1 = PasmFparamFromSillyString((char *)param1.c_str());
            line++; // skip past ','
            while (*line++ != ' '); // Skip over whitespace.
            std::string param2 = HandleStdStringUntil(&line, ",");
            struct pasm_fparam fparam2 = PasmFparamFromSillyString((char *)param2.c_str());
            line++; // skip past ','
            while (*line++ != ' '); // Skip over whitespace.
            SillyStringRemove0xA(line);
            char *fparam3 = MEMORY_ARENA.StringAlloc(line);
            pline.data_fptriad.param1 = fparam1;
            pline.data_fptriad.param2 = fparam2;
            pline.data_fptriad.param3 = fparam3;
            StretchyBufferPush(pasm_lines, pline);

        } else if (SillyStringStartsWith(line, "ret")) {
            
            pasm_line pline = PasmLineEmpty();
            pline.lineType = PASM_LINE_RET;
            StretchyBufferPush(pasm_lines, pline);

        } else if (SillyStringStartsWith(line, "mov") ||
            SillyStringStartsWith(line, "sub") ||
            SillyStringStartsWith(line, "add") ||
            SillyStringStartsWith(line, "xor"))
        {
            pasm_line pline = PasmLineEmpty();
            switch(*line) {
                // NOTE(Noah): It is probably going to be the case that we remove this at one point.
                // Because like, some instructions might start with the same register...
                case 'm':
                pline.lineType = PASM_LINE_MOV;
                break;
                case 's':
                pline.lineType = PASM_LINE_SUB;
                break;
                case 'a':
                pline.lineType = PASM_LINE_ADD;
                break;
                case 'x':
                pline.lineType = PASM_LINE_XOR;
                break;
                default:
                pline.lineType = PASM_LINE_UNDEFINED;
                break;
            }
            while (*line++ != ' '); // Skip over whitespace.
            std::string param1 = HandleStdStringUntil(&line, ",");
            struct pasm_fparam fparam1 = PasmFparamFromSillyString((char *)param1.c_str());
            line++; // skip past ','
            while (*line++ != ' '); // Skip over whitespace.
            SillyStringRemove0xA(line);
            struct pasm_fparam fparam2 = PasmFparamFromSillyString(line);
            pline.data_fptriad.param1 = fparam1;
            pline.data_fptriad.param2 = fparam2;
            StretchyBufferPush(pasm_lines, pline);

        }
    }

}

// Call to run the passembler as if it was from the command line
// inFile is the path to the file to run the assembler on.
// targetPlatform is not implemented at the moment.
int passembler(char *inFilePath, char *targetPlatform) {
    char *argv[3];
    argv[0] = "pplasm";
    argv[1] = inFilePath;
    argv[2] = targetPlatform;
    return pasm_main(3, argv);
}



#endif