/* TODO(Noah):

- Resolve stack variables.
- Adjust call to check first if function is defined as a definition, then 
  check for an external function.
- Actually do the proper work to care about the type of the parameter being
  passed. Like, is it int32, int64? Which one is it?


- Implement add instruction.
- Implement sub instruction.
- Implement branch_gt instruction.

- Implement mov instruction.

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
            case PASM_LINE_MOV:
            {
                // The first and last of these things are params.
                // params can be any of:
                // - int, label, register, stack label
                // 
                // semantically, what are the only valid first param types?
                // register.
                // - We also note that because of CISC, it could be the case 
                // that there are more valid locations than just a register.
                // 
                // TODO(Noah): Implement a semantic checking layer. Right now we assume
                // that the first param is going to be a register. If not, this line
                // evaluates to nothing.
                if (pline.data_fptriad.param1.type == PASM_FPARAM_REGISTER) {
                    char *cReg1 = 
                        pasmGprTable[(int)pline.data_fptriad.param1.data_register];
                    switch(pline.data_fptriad.param2.type) {
                        case PASM_FPARAM_REGISTER:
                        {
                            char *cReg2 = 
                                pasmGprTable[(int)pline.data_fptriad.param2.data_register];
                            fileWriter.write(SillyStringFmt("mov %s, %s\n", cReg1, 
                                cReg2));
                        }
                        break;
                        case PASM_FPARAM_INT:
                        fileWriter.write(SillyStringFmt("mov %s, %d\n", cReg1, 
                                pline.data_fptriad.param2.data_int));
                        break;
                        // TODO(Noah): And so what we do is we have this full-pass prior.
                        // The pass goes thru and finds that when there is a function def,
                        // we enter into a context.
                        // then, while we are inside this context (scope), for all labels not in the
                        // label table, we evaluate against the context. If we get a match,
                        // we change the PASM_FPARAM from LABEL to STACKVAR.
                        // while in the context, there also might be local vars created as well.
                        // these local vars get put inside the context as well. And can make it such that
                        // the label eval to the local var.
                        //
                        // what type of data do we even get inside the stack var?
                        // basically, we need to know the address of the stack variable.
                        // since these variables are always inside the context, they are relative
                        // to ebp. ebp+ for function params, and ebp- for locally created vars.
                        // AND, because we do it this way, we can account for diverse sized 
                        // variables on the stack. Yknow, maybe I throw an int32 on there. And then
                        // maybe we chuck an int8, int64. Heck, int16? Why not.
                        case PASM_FPARAM_STACKVAR:
                        {
                            // Suppose that things are pushed onto the stack.
                            // then we do a call into the function.
                            // there is going to exist rbp. rbp points to the old
                            // rpb, then next in memory is the return addr,
                            // and finally, we will have the params of the function.
                            // order of params depends on how we called the function
                            // in the first place.
                            //
                            // long story short, to get to the first param, we do
                            // [ebp + 8], presuming that each param is of a dword
                            // size...
                            //
                            // So. Maybe we do something like this...
                            struct pasm_stackvar sv = pline.data_fptriad.param2.data_sv;
                            fileWriter.write(SillyStringFmt("mov %s, [rbp %d]\n", cReg1,
                                sv.addr));
                        }
                        break;
                        case PASM_FPARAM_LABEL:
                        {
                            char *label = pline.data_fptriad.param2.data_cptr;
                            // TODO(Noah): Add checking for if in label_table.
                            fileWriter.write(SillyStringFmt("mov %s, %s\n", cReg1, 
                                label));
                        }
                        break;
                    }
                }
            }
            break;
            case PASM_LINE_SAVE:
            {
                for (int i = 0; i < StretchyBufferCount(pline.data_save); i++) {
                    char *cReg = pasmGprTable[(int)pline.data_save[i]];
                    fileWriter.write(SillyStringFmt("push %s\n", cReg));
                }
            }
            break;
            case PASM_LINE_RESTORE:
            {
                for (int i = 0; i < StretchyBufferCount(pline.data_save); i++) {
                    char *cReg = pasmGprTable[(int)pline.data_save[i]];
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