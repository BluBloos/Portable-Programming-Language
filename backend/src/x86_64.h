/* TODO(Noah):
- Implement branch_gt instruction.

- Ensure that parameters addrs are correct, given the order that they 
  are pushed onto the stack.

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

// NOTE(Noah): For this func below, this is an interesting case. We seem
// to have found a valid example for Polymorphism, but with structs.
// So this function does a thing, and there exists two structs. The thing depends 
// on just 1 param that is common to both structs. So this function can handle both,
// and not change anything to handle both. It is merely a language barrier that impedes
// the "clean" implementation of polymorphism. Below, I implement a small "hack" to get done 
// what needs to be done.
//
// This func returns the amount of bytes that were pushed to the stack.
// this is so that subsequent code can do the good job of restoring the 
// stack.
int FileWriter_WriteFunParamPassing(PFileWriter &fileWriter, 
    enum pasm_type *types, struct pasm_fnparam *_types, struct pasm_fparam *params) 
{
    int stackBytesPushed = 0;
    for (int i = 0; i < StretchyBufferCount(params); i++) {
        struct pasm_fparam fparam = params[i];
        enum pasm_type type = (_types != NULL) ? _types[i].type : types[i];
        char *reg;
        switch(type) {
            case PASM_INT8:
            case PASM_UINT8:
            // TODO(Noah): Implement.
            break;
            case PASM_INT16:
            case PASM_UINT16:
            // TODO(Noah): Implement.
            break;
            case PASM_INT32:
            case PASM_UINT32:
            reg = pasm_x64_GprTable[32];
            // stackBytesPushed += 4;
            break;
            case PASM_INT64:
            case PASM_UINT64:
            default:
            reg = pasm_x64_GprTable[0]; // default value
            // stackBytesPushed += 8;
            break;
        }
        switch(fparam.type) {
            case PASM_FPARAM_LABEL:
            fileWriter.write(SillyStringFmt("mov %s, %s\n", reg, fparam.data_cptr));
            break;
            case PASM_FPARAM_INT:
            fileWriter.write(SillyStringFmt("mov %s, %d\n", reg, fparam.data_int));
            break;
            case PASM_FPARAM_REGISTER:
            // NOTE(Noah): This is redundant in the output binary, but for code
            // readability / cleaness we do this.
            fileWriter.write(SillyStringFmt("mov %s, %s\n", reg, 
                pasm_x64_GprTable[(int)fparam.data_register]));
            break;
            case PASM_FPARAM_STACKVAR:
            fileWriter.write(SillyStringFmt("mov %s, ", reg));
            FileWriter_WriteStackVar(fileWriter, fparam.data_sv);
            fileWriter.write("\n");
            break;
        }

        // NOTE(Noah): In x64 we can only push 64 bit registers
        // onto the stack.
        // https://stackoverflow.com/questions/43435764/64-bit-mode-does-not-support-32-bit-push-and-pop-instructions
        fileWriter.write(SillyStringFmt("push rax\n"));
        stackBytesPushed += 8;
    }
    return stackBytesPushed;
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
            fileWriter.write("mov rsp, rbp\npop rbp\nret\n");
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
                fileWriter.write("push rbp\nmov rbp, rsp\n");
            }
            break;
            case PASM_LINE_LABEL:
            fileWriter.write(SillyStringFmt("%s:\n", pline.data_cptr));
            break;
            case PASM_LINE_FCALL:
            {
                struct pasm_fcall fcall = pline.data_fcall;
                
                // Reading documentation for stbds here: http://nothings.org/stb_ds/
                // We should be using stbds_shgeti - for a string hash map.
                // string hash maps come with faculties for storing strings.
                //
                // but we do not need to do this as we manage our own strings in 
                // our own form of permenant storage.

                // Is the function an fdecl, or an fdef?
                char *fname = fcall.name;
                int sbp = 0;
                if (stbds_shgeti(fdecl_table, fname) != -1) {
                    // dealing with an fdecl.
                    struct pasm_fdecl fdecl = stbds_shget(fdecl_table, fname);
                    sbp = FileWriter_WriteFunParamPassing(fileWriter, fdecl.params, NULL, fcall.params);
                     
                } else if (stbds_shgeti(fdef_table, fname) != -1) {
                    // dealing with an fdef. 
                    struct pasm_fdef fdef = stbds_shget(fdef_table, fname);
                    sbp = FileWriter_WriteFunParamPassing(fileWriter, NULL, fdef.params, fcall.params);

                } else {
                    // Error!
                    LOGGER.Error(
                        "PASM_LINE_CALL trying to call %s, \
                        but this func cannot be found in either fdecl_table\
                        or fdef_table", fname);
                    pasm_x86_64_result = 1;
                    goto pasm_x86_64_end;
                }

                // Call the function
                fileWriter.write(SillyStringFmt("call %s\n", fcall.name));
                // Restore the stack.
                fileWriter.write(SillyStringFmt("add rsp, %d\n", sbp));
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