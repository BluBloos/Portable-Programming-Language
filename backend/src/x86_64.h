/* TODO(Noah):


- Implement branch_gt instruction.

- Adjust call to check first if function is defined as a definition, then 
  check for an external function.
- Also adjust call to do the proper pushing onto the stack.

*/

char *pasm_x64_GprTable[] = {
    "rax", "rbx", "rcx", "rdx",
    "rsp", "rbp", "rsi", "rdi",
    "r8", "r9", "r10", "r11",
    "r12", "r13", "r14", "r15",
    "", "", "", "", // TODO(Noah): Implement more registers?
    "", "", "", "",
    "", "", "", "",
    "", "", "", "",
    "eax", "ebx", "ecx", "edx",
    "esp", "ebp", "esi", "edi",
    "r8d", "r9d", "r10d", "r11d",
    "r12d", "r13d", "r14d", "r15d"
    // TODO(Noah): Implement 16 bit and 8 bit registers.
};

// p assembly function call param order.
int pasmfcallpo[] = {
    2, // rcx
    3, // rdx
    8, // r8
    9 // r9
};

/* Write using a PFileWriter the x86 assembly for the stack variable. */
void FileWriter_WriteStackVar(PFileWriter &fileWriter, struct pasm_stackvar sv) {
    switch(sv.type) {
        case PASM_INT8:
        case PASM_UINT8:
        fileWriter.write("BYTE ");
        break;
        case PASM_INT16:
        case PASM_UINT16:
        fileWriter.write("WORD "); // word on x86_64 is 16 bits?
        break;
        case PASM_INT32:
        case PASM_UINT32:
        fileWriter.write("DWORD ");
        break;
        case PASM_INT64:
        case PASM_UINT64:
        fileWriter.write("QWORD ");
        break;
        default:
        break;
    }
    if (sv.addr >= 0) {
        fileWriter.write(SillyStringFmt("[rbp + %d]", sv.addr));
    } else {
        fileWriter.write(SillyStringFmt("[rbp %d]", sv.addr));
    }
    
}

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
            case PASM_LINE_ADD:
            case PASM_LINE_SUB:
            case PASM_LINE_MOV:
            {
                bool add_sub_Flag = false;
                switch(pline.lineType) {
                    case PASM_LINE_ADD:
                    fileWriter.write("add ");
                    add_sub_Flag = true;
                    break;
                    case PASM_LINE_SUB:
                    fileWriter.write("sub ");
                    add_sub_Flag = true;
                    break;
                    case PASM_LINE_MOV:
                    fileWriter.write("mov ");
                    break;
                    default:
                    break;
                }
                bool firstParamValid = false;                
                if (pline.data_fptriad.param1.type == PASM_FPARAM_REGISTER) {
                    firstParamValid = true;
                    char *cReg1 = 
                        pasm_x64_GprTable[(int)pline.data_fptriad.param1.data_register];
                    fileWriter.write(SillyStringFmt("%s, ", cReg1));
                } else if (pline.data_fptriad.param1.type == PASM_FPARAM_STACKVAR) {
                    firstParamValid = true;
                    FileWriter_WriteStackVar(fileWriter, 
                        pline.data_fptriad.param1.data_sv);
                    fileWriter.write(", ");
                }
                if (firstParamValid) {
                    switch(pline.data_fptriad.param2.type) {
                        case PASM_FPARAM_REGISTER:
                        {
                            char *cReg2 = 
                                pasm_x64_GprTable[(int)pline.data_fptriad.param2.data_register];
                            fileWriter.write(SillyStringFmt("%s\n", cReg2));
                        }
                        break;
                        case PASM_FPARAM_INT:
                        fileWriter.write(SillyStringFmt("%d\n", 
                            pline.data_fptriad.param2.data_int));
                        break;
                        case PASM_FPARAM_STACKVAR:
                        FileWriter_WriteStackVar(fileWriter, 
                            pline.data_fptriad.param2.data_sv);
                        fileWriter.write("\n");
                        break;
                        case PASM_FPARAM_LABEL:
                        {
                            char *label = pline.data_fptriad.param2.data_cptr;
                            // TODO(Noah): Add checking for if in label_table.
                            fileWriter.write(SillyStringFmt("%s\n", label));
                        }
                        break;
                    }
                }       
            }
            break;
            // NOTE(Noah): Save and restore instructions always just save/restore 
            // the entire 64 bit register.
            case PASM_LINE_SAVE:
            {
                for (int i = 0; i < StretchyBufferCount(pline.data_save); i++) {
                    char *cReg = pasm_x64_GprTable[(int)pline.data_save[i]];
                    fileWriter.write(SillyStringFmt("push %s\n", cReg));
                }
            }
            break;
            case PASM_LINE_RESTORE:
            {
                for (int i = 0; i < StretchyBufferCount(pline.data_save); i++) {
                    char *cReg = pasm_x64_GprTable[(int)pline.data_save[i]];
                    fileWriter.write(SillyStringFmt("pop %s\n", cReg));
                }
            }
            break;
            case PASM_LINE_BRANCH:
            fileWriter.write(SillyStringFmt("jmp %s\n", pline.data_cptr));
            break;
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
                /* for (int i = 0; i < StretchyBufferCount(fcall.params); i++) {
                    struct pasm_fparam fparam = fcall.params[i];
                    switch(fparam.type) {
                        case PASM_FPARAM_LABEL:
                        fileWriter.write(SillyStringFmt("%s\n", fparam.data_cptr));
                        break;
                        case PASM_FPARAM_INT:
                        fileWriter.write(SillyStringFmt("%d\n", fparam.data_int));
                        break;
                        case PASM_FPARAM_REGISTER:
                        fileWriter.write(SillyStringFmt("%s\n", cReg,
                            pasm_x64_GprTable[(int)fparam.data_register]));
                        break;
                        case PASM_FPARAM_STACKVAR:
                        FileWriter_WriteStackVar(fileWriter, fparam.data_sv);
                        fileWriter.write("\n");
                        break;
                    }

                } */
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