/* TODO(Noah):
- Resolve stack variables.
- Adjust call to check first if function is defined as a definition, then 
  check for an external function.
- Actually do the proper work to care about the type of the parameter being
  passed. Like, is it int32, int64? Which one is it?
- Implement return instruction.
- Implement restore instruction.
- Implement branch instruction.
- Implement add instruction.
- Implement sub instruction.
- Implement mov instruction.
- Implement branch_gt instruction. 
- Implement save instruction.
*/

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

// This function will write x86_64 assembly source to outFilePath
// by translating the in-memory representation of PASM (ppl assembly)
// as stored in source. Source is a stretchy buffer. 
int pasm_x86_64(struct pasm_line *source, 
    char *outFilePath, enum target_platform tplat) {
    
    int pasm_x86_64_result = 0;
    PFileWriter fileWriter = PFileWriter(outFilePath);

    for (int i = 0 ; i < StretchyBufferCount(source); i++) {
        struct pasm_line pline = source[i];
        switch(pline.lineType) {
            case PASM_LINE_RET:
            fileWriter.write("ret\n");
            break;
            case PASM_LINE_FDECL: 
            {
                char *fname = pline.data_fdecl.name;
                fileWriter.write(SillyStringFmt("extern %s\n", fname));
            }
            break;
            case PASM_LINE_SECTION_CODE:
            fileWriter.write("section .text\n");
            break;
            case PASM_LINE_SECTION_DATA:
            fileWriter.write("section .data\n");
            break;
            case PASM_LINE_FDEF:
            {
                char *fname = pline.data_fdef.name;
                if (tplat == MAC_OS && 
                    SillyStringStartsWith("main", fname) ) 
                {
                    fileWriter.write("global start\nstart:\n");
                } else {
                    fileWriter.write(SillyStringFmt("%s:\n", fname));
                }
            }
            break;
            case PASM_LINE_LABEL:
            fileWriter.write(SillyStringFmt("%s:\n", pline.data_cptr));
            break;
            case PASM_LINE_FCALL:
            {
                struct pasm_fcall fcall = pline.data_fcall;
                // Before we call the function, we have to setup the params.
                // and before we can do anything, we need to know the header definition
                // of the function that we are calling.
                char *fname = fcall.name;
                // TODO(Noah): Figure out why stbds_hmgeti is NOT FUCKING WORKING.
                // Like. You gotta love libraries man...
                if ( false && (stbds_hmgeti(fdecl_table, fname)) == -1 ) {
                    // Error!
                    LOGGER.Error(
                        "PASM_LINE_CALL trying to call %s, \
                        but this func is not defined in fdecl_table", fname);

                    for (int i=0; i < stbds_hmlen(fdecl_table); ++i) {
                        LOGGER.Min("%s ", fdecl_table[i].key);
                        struct pasm_line pline = PasmLineEmpty();
                        pline.lineType = PASM_LINE_FDECL;
                        pline.data_fdecl = fdecl_table[i].value;
                        PasmLinePrint(pline);
                    }

                    pasm_x86_64_result = 1;
                    goto pasm_x86_64_end;
                }
                
                struct pasm_fdecl fdecl = stbds_hmget(fdecl_table, fname); 
                // TODO(Noah): Implement different calling conventions. Right now
                // we only implement just 1.
                // we also do not check to see if the func has any more than 4 parameters...
                for (int i = 0; i < StretchyBufferCount(fcall.params); i++) {
                    struct pasm_fparam fparam = fcall.params[i];
                    char *cReg = pasmGprTable[pasmfcallpo[i]];
                    switch(fparam.type) {
                        case PASM_FPARAM_LABEL:
                        fileWriter.write(SillyStringFmt("mov %s, %s\n", cReg, fparam.data_cptr));
                        break;
                        case PASM_FPARAM_INT:
                        fileWriter.write(SillyStringFmt("mov %s, %d\n", cReg, fparam.data_int));
                        break;
                        case PASM_FPARAM_REGISTER:
                        fileWriter.write(SillyStringFmt("mov %s, %s\n", cReg,
                            pasmGprTable[(int)fparam.data_register]));
                        break;
                    }

                }
                // Call the function
                fileWriter.write(SillyStringFmt("call %s\n", fcall.name));
            }
            break;
            case PASM_LINE_DATA_BYTE_STRING:
            fileWriter.write(
                SillyStringFmt("db \"%s\"\n", pline.data_cptr));
            break;
            case PASM_LINE_DATA_BYTE_INT:
            fileWriter.write(
                SillyStringFmt("db %d\n", pline.data_int));
            break;
            default:
            break;
        }
    }

    pasm_x86_64_end:
    return pasm_x86_64_result;
}