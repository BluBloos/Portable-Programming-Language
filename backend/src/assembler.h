#ifndef PASSEMBLER_H
#define PASSEMBLER_H
#include <vector>

enum pasm_line_type {
    PASM_LINE_UNDEFINED = 0,
    PASM_LINE_SECTION_CODE,
    PASM_LINE_SECTION_DATA,
    PASM_LINE_LABEL
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
    };
};

struct pasm_line PasmLineEmpty() {
    struct pasm_line pl;
    pl.lineType = PASM_LINE_UNDEFINED;
    pl.data_cptr = (char *)0;
    return pl;
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
    }
}
#endif