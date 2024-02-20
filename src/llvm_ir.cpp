// this is the backend to convert PASM to LLVM-IR.

// code written here is likely copy-pasta from 
// https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl03.html#id3

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm-c/TargetMachine.h"
#include "lld/Common/Driver.h"


// This function will build the LLVM IR into builder by translating
// the in-memory representation of PASM (ppl assembly) as stored in source.
int pasm_llvm_ir(struct pasm_line *source, llvm::LLVMContext &TheContext, llvm::Module &TheModule, llvm::IRBuilder<> &builder)
{
    int result = 0;

    for (int i = 0 ; i < StretchyBufferCount(source); i++) {
        struct pasm_line pl = source[i];
        switch(pl.lineType) {
            case PASM_LINE_LET:
            {
                // TODO:
                // LOGGER.Min("  type:"); PasmTypePrint(pl.data_let.type);

                const char *VarName = pl.data_let.name;
                builder.CreateAlloca(llvm::Type::getDoubleTy(TheContext), nullptr, VarName);
            } break;
            case PASM_LINE_UNLET:
            {
                // TODO: tutorial did not appear to indicate what to do for dealloc. check documentation.
            } break;
            case PASM_LINE_SETNZ:
            break;
            // double parameter instructions.
            case PASM_LINE_ADD:
            case PASM_LINE_SUB:
            case PASM_LINE_MOV:
            case PASM_LINE_AND:
            case PASM_LINE_XOR:
            case PASM_LINE_MOVSX:
            case PASM_LINE_MOVNZ:
            case PASM_LINE_CMP:
            {
                // assume that both L and R are stack variables.
                
                // L and R are the references returned from the alloca.
                llvm::Value *L = nullptr;
                llvm::Value *R = nullptr;
                
                if (!L || !R)
                    break;
                
                llvm::Value *Lval = builder.CreateLoad(llvm::Type::getDoubleTy(TheContext), L );
                llvm::Value *Rval = builder.CreateLoad(llvm::Type::getDoubleTy(TheContext), R );
                
                llvm::Value *result;

                switch (pl.lineType) {
                    case PASM_LINE_ADD: result = builder.CreateFAdd(Lval, Rval ); break;
                    case PASM_LINE_SUB: result = builder.CreateFAdd(Lval, Rval ); break;
                    case PASM_LINE_MOV: result = builder.CreateFAdd(Lval, Rval ); break;
                    case PASM_LINE_AND: result = builder.CreateFAdd(Lval, Rval ); break;
                    case PASM_LINE_XOR: result = builder.CreateFAdd(Lval, Rval ); break;
                    case PASM_LINE_MOVSX: result = builder.CreateFAdd(Lval, Rval ); break;
                    case PASM_LINE_MOVNZ: result = builder.CreateFAdd(Lval, Rval ); break;
                    case PASM_LINE_CMP: result = builder.CreateFAdd(Lval, Rval ); break;
                }

                builder.CreateStore(result, L);

            } break;
            case PASM_LINE_SAVE:{
                llvm::Value *savedThing;
            }
            break;
            case PASM_LINE_RESTORE:{
                llvm::Value *restoredThing;
                // TODO: find the savedThing and use that to assign
                // and thus create restoredThing.
            }
            break;
            //NOTE; One interesting (and very important) aspect of the LLVM
            // IR is that it requires all basic blocks to be “terminated” with a 
            //control flow instruction such as return or branch.
            case PASM_LINE_BRANCH_GT:
            case PASM_LINE_BRANCH_GTE:
            case PASM_LINE_JUMP_EQ:
            {
                llvm::Value *CondV = nullptr; // TODO: create an expression and generate code for that to get value.
                llvm::BasicBlock * ThenBB = nullptr; // TODO: use the label to lookup the basic block.
                
                // TODO: is it OK to jump to curr BB?
                llvm::BasicBlock *currBB = builder.GetInsertBlock();
                builder.CreateCondBr(CondV, ThenBB, currBB);
            } break;
            // TODO: because of SSA, phi nodes are required to indicate
            // a value which could be different depending on the control flow
            // of the past. the phi nodes selects between two inputs using
            // the control flow to make the selection.
            //
            // BUT, LLVM-IR also has the notion of a stack var. simply alloca
            // to create one and we can load and store to modify.
            // no need for phi nodes. there is a mem2reg pass that will promote
            // to SSA.
            case PASM_LINE_BRANCH:
            {
                llvm::BasicBlock *targetBB = nullptr; // TODO: use label to lookup basic block.
                builder.CreateBr(targetBB);
            } break;
            case PASM_LINE_RET:
            {
                /*
                 NOTE; for purpose of generate LLVM-IR; we store everything in stack variables.
                 we treat registers as stack variables.
                 if the save thing is called, that's as if we are creating stack var for that register.
                 we also create upfront stack vars for the reserved registers.
                 */
                
                llvm::Value *RetVal = nullptr; // TODO: load value from "r2 stackvar".
                
                // Finish off the function.
                builder.CreateRet(RetVal);

                // TODO: PASM will have multiple exit points of the function.
                // for each of those, we will emit Ret instr. but we only want
                // to validate the function code once.
                //
                // Validate the generated code, checking for consistency.
                // verifyFunction(*TheFunction);
            } break;
            case PASM_LINE_SECTION_CODE:
            case PASM_LINE_SECTION_DATA:
            {
                // NOTE: nothing to emit here for LLVM backend.
            }
            break;
            case PASM_LINE_FDECL: // function prototype.
            {
                 // Make the function type:  double(double,double) etc.
                std::vector<llvm::Type*> Doubles(2,
                                                 llvm::Type::getDoubleTy(TheContext));
                llvm::FunctionType *FT =
                llvm::FunctionType::get(llvm::Type::getDoubleTy(TheContext), Doubles, false);

                llvm::Function *F =
                llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "function", TheModule);
            }
            break;
            // NOTE: for a function definition, FDEF is used instead of LABEL.
            case PASM_LINE_FDEF:
            {
                llvm::Function *F = nullptr; // TODO: use name to lookup function.
                
                llvm::BasicBlock *BB = llvm::BasicBlock::Create(TheContext, "entry", F);
                builder.SetInsertPoint(BB);
            } break;
            case PASM_LINE_LABEL:
            {
                llvm::BasicBlock *BB = llvm::BasicBlock::Create(TheContext, "label");
                builder.SetInsertPoint(BB);
            }
            break;
            case PASM_LINE_FCALL:
            {
                std::vector<llvm::Value *> ArgsV;
                for (unsigned i = 0, e = 10; i != e; ++i) {
                  ArgsV.push_back( nullptr ); // make this work.
                }
                
                llvm::Function *F = nullptr; // TODO: use name to lookup function.
                llvm::Value*v=builder.CreateCall(F, ArgsV); // TODO: store result into "r2 stackvar".
            } break;
            case PASM_LINE_DATA_BYTE_STRING:
            {
                // TODO: register this under the appropriate label.
                llvm::Value *v = llvm::ConstantFP::get(TheContext, llvm::APFloat(0.0));
            } break;
            case PASM_LINE_DATA_BYTE_INT:
            {
                // TODO: register this under the appropriate label.
                llvm::Value *v = llvm::ConstantFP::get(TheContext, llvm::APFloat(0.0));
            } break;
            default:
            PPL_TODO;
            break;
        }
    }

    pasm_llvm_ir_end:

    return result;
}

// TODO: make able to change the filename here.
int llvm_ir_to_object(llvm::Module &TheModule, bool bOutputBinary)
{
    // Initialize the target registry etc.
    LLVMInitializeX86AsmParser();
    LLVMInitializeX86Target();
    LLVMInitializeX86TargetMC();
    LLVMInitializeX86AsmParser();
    LLVMInitializeX86AsmPrinter();
    
    // get the target triple for host machine.
    auto TargetTriple = LLVMGetDefaultTargetTriple();

    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

    if (!Target) {
        PPL_TODO; // consider this a bug. shouldn't ever happen for end user.
        return 1;
    }

    auto CPU = "generic";
    auto Features = "";
    llvm::TargetOptions opt;
    auto TargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, llvm::Reloc::PIC_);

    std::string Filename = "myProgram.o";
    std::error_code EC;
    llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::OF_None);

    if (EC) {
        //errs() << "Could not open file: " << EC.message();
        LOGGER.Error("Could not open %s. Maybe disk is out of space?", Filename.c_str());
        return 1;
    }

    llvm::legacy::PassManager pass;
    auto FileType = llvm::CGFT_ObjectFile;

    if (TargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
        //errs() << "TargetMachine can't emit a file of this type";
        PPL_TODO; // consider this a bug. shouldn't ever happen for end user.
        return 1;
    }

    pass.run(TheModule);
    dest.flush();

    if (bOutputBinary) {
        
        std::vector<const char *> args = {
            "ld64.lld",
            "-o",
            "myProgram",
            "-static",
            "myProgram.o",
            PSTDLIB_UNIX_DIR "/macOS/exit.o",
            PSTDLIB_UNIX_DIR "/macOS/console/print.o",
            PSTDLIB_UNIX_DIR "/macOS/stub.o"
        };

        bool exitEarly=false;
        bool disableOutput=false;
        bool r=lld::macho::link(  args, llvm::outs(), llvm::errs(), exitEarly, disableOutput); 

    } else {
        return 0;
    }
}
