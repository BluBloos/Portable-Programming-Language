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
    PASM_LINE_FDECL
};

// calling convention
enum pasm_cc {
    PASM_CC_PDECL = 0,
    PASM_CC_CDECL
};

// TODO(Noah): I'm pretty sure it is feasible to have 128 bit floats
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
    enum pasm_type *params; // stretchy buffer.
};

struct pasm_line {
    enum pasm_line_type lineType;
    union {
        char *data_cptr;
        struct pasm_fdecl data_fdecl;
    };
};

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
    return pl;
}

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
    }
}

void HandleLine(char *line);
struct pasm_line *pasm_lines = NULL; // stretchy buffer.

// USAGE:
// pplasm <inFile> <targetPlatform> 
int pasm_main(int argc, char **argv) {

    // StretchyBufferInit(pasm_lines);

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

    for (int i = 0; i < StretchyBufferCount(pasm_lines); i++) {
        PasmLinePrint(pasm_lines[i]);
    }

    for (int i = 0; i < StretchyBufferCount(pasm_lines); i++) {
        struct pasm_line pline = pasm_lines[i];
        if (pline.lineType == PASM_LINE_FDECL) {
            StretchyBufferFree(pline.data_fdecl.params);
        }
    }

    StretchyBufferFree(pasm_lines);

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
        } 
        // NOTE(Noah): I suppose the only thing that will ever be extern
        // is that we are trying to define a function that someone who is 
        // not us can call.
        else if (directive == "extern") {

            pasm_line pline = PasmLineEmpty();
            pline.lineType = PASM_LINE_FDECL;    

            // First thing to check is the calling convention.
            enum pasm_cc ecc = (*line == 'p') ? PASM_CC_PDECL : 
                PASM_CC_CDECL;
            while (*line++ != ' '); // Skip over until whitespace.
            if (ecc == PASM_CC_PDECL && VERBOSE) {
                LOGGER.Log("p_decl");
            } else {
                LOGGER.Log("c_decl");
            }
            pline.data_fdecl.callingConvention = ecc;

            // Now check the return type.
            enum pasm_type ptype = PASM_VOID;
            std::string type = "";
            while (*line != ' ') type += *line++;
            ptype = SillyStringGetPasmType((char *)type.c_str());
            if(VERBOSE) { 
                PasmTypePrint(ptype);
            }
            pline.data_fdecl.returnType = ptype;

            line++; // Skip over ' '

            // Now check the name for the function.
            std::string fname = "";
            while(*line != '(') fname += *line++;
            if (VERBOSE) {
                LOGGER.Min("fname: %s\n", fname.c_str());
            }
            pline.data_fdecl.name = MEMORY_ARENA.StdStringAlloc(fname);

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
            // TODO(Noah): Implement.
        }
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