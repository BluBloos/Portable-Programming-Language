enum pasm_line_type {
    PASM_LINE_UNDEFINED = 0,
    PASM_LINE_SECTION_CODE,
    PASM_LINE_SECTION_DATA
};

struct pasm_line {
    enum pasm_line_type lineType;
};

struct pasm_line PasmLineEmpty() {
    struct pasm_line pl;
    pl.lineType = PASM_LINE_UNDEFINED;
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
    }
}