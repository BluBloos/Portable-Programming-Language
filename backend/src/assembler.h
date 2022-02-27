// This file takes the human readable text representation of the PPL IR
// and converts it into an in-memory representation suitable for translation into
// arbitrary target platforms such as x86_64, x86, 64 bit ARM, webassmely, and
// generally, and RISC / CISC based architecture.

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
    PASM_LINE_FDEF
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
    PASM_FLOAT64
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
    PASM_FPARAM_REGISTER
};

enum pasm_register {
    PASM_R0 = 0, PASM_R1,
    PASM_R2, PASM_R3,
    PASM_R4, PASM_R5,
    PASM_R6, PASM_R7,
    PASM_R8, PASM_R9,
    PASM_R10, PASM_R11,
    PASM_R12, PASM_R13,
    PASM_R14, PASM_R15,
    PASM_R16, PASM_R17,
    PASM_R18, PASM_R19,
    PASM_R20, PASM_R21,
    PASM_R22, PASM_R23,
    PASM_R24, PASM_R25,
    PASM_R26, PASM_R27,
    PASM_R28, PASM_R29,
    PASM_R30, PASM_R31,
};

// Function params
struct pasm_fparam {
    enum pasm_fparam_type type;
    union {
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

struct pasm_line {
    enum pasm_line_type lineType;
    union {
        char *data_cptr;
        struct pasm_fdecl data_fdecl;
        struct pasm_fcall data_fcall;
        struct pasm_fdef data_fdef;
        int data_int;
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

struct pasm_func_table {
    char *key;
    struct pasm_fdecl value;
};

char *pasmGprTable[] = {
    "rax", "rbx",
    "rcx", "rdx",
    "rsp", "rbp",
    "rsi", "rdi",
    "r8", "r9",
    "r10", "r11",
    "r12", "r13",
    "r14", "r15"
};

// p assembly function call param order.
int pasmfcallpo[] = {
    2, // rcx
    3, // rdx
    8, // r8
    9 // r9
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
    }
}

void PasmLinePrint(struct pasm_line pl) {
    switch(pl.lineType) {
        case PASM_LINE_UNDEFINED:
        LOGGER.Min("PASM_LINE_UNDEFINED\n");
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
        case PASM_LINE_FDECL:
        {
            LOGGER.Min("PASM_LINE_FDECL\n");
            LOGGER.Min("  name:%s\n", pl.data_fdecl.name);
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
        case PASM_LINE_FCALL:
        {
            LOGGER.Min("PASM_LINE_FCALL\n");
            LOGGER.Min("  name:%s\n", pl.data_fcall.name);
            LOGGER.Min("  params:\n");
            for (int i = 0; i < StretchyBufferCount(pl.data_fcall.params); i++) {
                //LOGGER.Min("    "); PasmTypePrint(pl.data_fdecl.params[i]);
                struct pasm_fparam fparam = pl.data_fcall.params[i];
                switch(fparam.type) {
                    case PASM_FPARAM_INT:
                    LOGGER.Min("    PASM_FPARAM_INt\n");
                    LOGGER.Min("      %d\n", fparam.data_int);
                    break;
                    case PASM_FPARAM_LABEL:
                    LOGGER.Min("    PASM_FPARAM_LABEL\n");
                    LOGGER.Min("      %s\n", fparam.data_cptr);
                    break;
                    case PASM_FPARAM_REGISTER:
                    LOGGER.Min("    PASM_FPARAM_REGISTER\n");
                    LOGGER.Min("      r%d\n", (int)fparam.data_register);
                    break;
                }
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
    unsigned int num = SillyStringToUINT(str + 1);    
    if (num >= 0 && num < 32) {
        reg = (enum pasm_register)((int)PASM_R0 + num);
        return true;
    }
    return false;
}

// Deallocs the lines in the stretchy buffer lines.
void DeallocPasmLines(struct pasm_line *lines) {
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
            default:
            break;
        }
    }
    StretchyBufferFree(lines);
}

void HandleLine(char *line);
struct pasm_line *pasm_lines = NULL; // stretchy buffer.

// USAGE:
// pplasm <inFile> <targetPlatform> 
int pasm_main(int argc, char **argv) {

    // StretchyBufferInit(pasm_lines);
    pasm_lines = NULL;

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
        for (int i = 0; i < StretchyBufferCount(pasm_lines); i++) {
            PasmLinePrint(pasm_lines[i]);
        }
    }

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
    if(VERBOSE) { 
        PasmTypePrint(ptype);
    }
    *pline = line;
    return ptype;
}

/* Takes in a pointer to a silly string that is assumed to start at some
   generic word. Also takes in a terminating character. The function will
   parse the generic word, terminating at cTerm. The underlying silly string
   will be advanced. */
char *HandleUntil(char **pline, char cTerm) {
    char *line = *pline;
    std::string name = "";
    while(*line != cTerm) name += *line++;
    *pline = line;
    return MEMORY_ARENA.StdStringAlloc(name);
}

void HandleLine(char *line) {
    
    // NOTE(Noah): For right now, we are literally just going to echo the lines of all source files.
    // And we know that all lines contain the null-terminator, so we are going to use the Min version
    // of the log function.
    LOGGER.Min("%s", line);

    if (*line == '.') { // Found a directive.
        
        line++; // skip over the '.'
        std::string directive = "";
        // go up until the space.
        while (*line != ' ') {
            directive += *line++;
        }
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
            pline.data_fdef.name = HandleUntil(&line, '(');
            line++; // Skip over '('

            // Now we have to check for the parameters.
            while(*line != ')') {
                while (*line == ',' || *line == ' ') { line++; }
                std::string _type = "";
                while(*line != ' ') {
                    _type += *line++;
                }
                enum pasm_type ptype = SillyStringGetPasmType(
                    (char *)_type.c_str());

                line++; // skip past ' '

                // Parse param name.
                std::string pname = "";
                while(*line != ',' && *line != ')') {
                    pname += *line++;
                }

                struct pasm_fnparam fnparam;
                fnparam.name = MEMORY_ARENA.StdStringAlloc(pname);
                fnparam.type = ptype;

                StretchyBufferPush(pline.data_fdef.params, fnparam);
                // *line++; // skip over ','
                // *line++; // skip over ' '                
            }

            StretchyBufferPush(pasm_lines, pline);

            
        } else if (directive == "extern") {

            pasm_line pline = PasmLineEmpty();
            pline.lineType = PASM_LINE_FDECL;    

            // First thing to check is the calling convention.
            {
                enum pasm_cc ecc = (*line == 'p') ? PASM_CC_PDECL : 
                PASM_CC_CDECL;
                while (*line++ != ' '); // Skip over until whitespace.
                if (ecc == PASM_CC_PDECL && VERBOSE) {
                    LOGGER.Log("p_decl");
                } else {
                    LOGGER.Log("c_decl");
                }
                pline.data_fdecl.callingConvention = ecc;
            }

            pline.data_fdecl.returnType = HandleType(&line);
            line++; // Skip over ' '
            pline.data_fdecl.name = HandleUntil(&line, '(');
            line++; // Skip over '('

            // Now we have to check for the parameters.
            while(*line != ')') {
                while (*line == ',' || *line == ' ') { line++; }
                std::string _type = "";
                while(*line != ',' && *line != ')') {
                    _type += *line++;
                }
                enum pasm_type ptype = SillyStringGetPasmType(
                    (char *)_type.c_str());
                if (VERBOSE) PasmTypePrint(ptype);
                StretchyBufferPush(pline.data_fdecl.params, ptype);
                // *line++; // skip over ','
                // *line++; // skip over ' '                
            }

            StretchyBufferPush(pasm_lines, pline);

        } else if (directive == "db") {

            pasm_line pline = PasmLineEmpty();
            // TODO(Noah): Add more types of byte literals.
            //   Ex) Add negative integers.
            std::string strLiteral = "";
            if (*line == '"') {
                pline.lineType = PASM_LINE_DATA_BYTE_STRING;
                line++; // skip over the "
                while (*line != '"') {
                    strLiteral += *line++;
                }
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
        std::string label = "";
        while (*line != ':') {
            label += *line++;
        }
        char *pstr = MEMORY_ARENA.StdStringAlloc(label);
        pasm_line pline = PasmLineEmpty();
        pline.lineType = PASM_LINE_LABEL;
        pline.data_cptr = pstr;
        StretchyBufferPush(pasm_lines, pline);

    } else {
        // We can now make the assumption that we are deadling
        // with a full-blown assembly command. So it's got the
        // pneumonic and everything.
        if (SillyStringStartsWith(line, "call")) {
            pasm_line pline = PasmLineEmpty();
            pline.lineType = PASM_LINE_FCALL;
    
            while (*line++ != ' ');
            pline.data_fcall.name = HandleUntil(&line, '(');
            line++; // skip past the '('

            // Now we have to check for the parameters.
            while(*line != ')') {
                while (*line == ',' || *line == ' ') { line++; }
                
                // Function params are either going to be an immediate
                //   which would be a number of a label.
                // OR, function params are going to be a register.
                std::string param = "";
                while(*line != ',' && *line != ')') {
                    param += *line++;
                }

                struct pasm_fparam fparam;
                bool _dflag;
                enum pasm_register _reg;
                if (SillyStringGetRegister(
                    (char *)param.c_str(), _reg)) 
                {
                    fparam.type = PASM_FPARAM_REGISTER;
                    fparam.data_register = _reg;
                } else if (SillyStringIsNumber((char *)param.c_str(),
                    _dflag)) 
                {
                    fparam.type = PASM_FPARAM_INT;
                    fparam.data_int = SillyStringToUINT(
                        (char *)param.c_str());
                } else {
                    // We know it's a label.
                    fparam.type = PASM_FPARAM_LABEL;
                    fparam.data_cptr = MEMORY_ARENA.StringAlloc(
                        (char *)param.c_str()
                    );
                }
                StretchyBufferPush(pline.data_fcall.params, fparam);
                            
            }

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