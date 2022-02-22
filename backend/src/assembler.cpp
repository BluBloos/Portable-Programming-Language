// This file takes the human readable text representation of the PPL IR
// and converts it into an in-memory representation suitable for translation into
// arbitrary target platforms such as x86_64, x86, 64 bit ARM, webassmely, and
// generally, and RISC / CISC based architecture.

#include <ppl_util.h>
#include "assembler.h"
#include <vector>

void HandleLine(char *line);

struct pasm_line *pasm_lines = NULL; // stretchy buffer.

// USAGE:
// pplasm <inFile> <targetPlatform> 
int main(int argc, char **argv) {

    StretchyBufferInit(pasm_lines);

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
        }

    }

    for (int i = 0; i < StretchyBufferCount(pasm_lines); i++) {
        PasmLinePrint(pasm_lines[i]);
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

            /*
            // First thing to check is the calling convention
            enum pasm_cc ecc = (*line == 'p') ? PASM_CC_PDECL : 
                PASM_CC_CDECL;
            while (*line++ != ' '); // skip over until whitespace

            // Now check the return type.
            enum pasm_type ptype = PASM_VOID;
            std::string type = "";
            while (*line != ' ') type += *line++;
            ptype = SillyStringGetPasmType((char *)type.c_str());

            // Now check the name for the function.
            std::string fname = "";
            while(*line != '(') fname += *line++;

            // Now we have to check for the parameters.
            while(*line != ')') {
                while (*line == ',' || *line == ' ') line++;
                std::string type = "";
                while(*line != ',' || *line != ')') {
                    type += *line++;
                }
                enum pasm_type ptype = SillyStringGetPasmType((char *)type.c_str());
                // *line++; // skip over ','
                // *line++; // skip over ' '
                
            }
            */

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