#ifndef CODEGEN_H
#define CODEGEN_H

#define LLVM_IR_BACKEND

/*
So frankly, there is some spooky action here.
We actually need to get this generating just ONE thing.
The hook architecture I do not think is going to work.
    - different platforms are so speficic.

So we have to go down to the LLVM IR first, then to different platforms from there.

Now support we are doing the web target.
    -> we can compile LLVM IR right down to web assembly.
    -> but what if we want to do inline javascript??
        -> how does this work?
        -> it bypasses the LLVM IR layer and "links" with the compiled webasm.
*/


// So, we're going to need to be able to suppport multiple target platforms
// via this file.
// this is where all the magic of PPL happens :)

class SpecialFilehandle {
public:
    FILE *handle;
    unsigned int indentation;
    bool fresheNewLine;
    SpecialFilehandle(FILE *handle) : handle(handle), indentation(0), fresheNewLine(true) {}
    ~SpecialFilehandle() { handle.close(); }
    void IncreaseIndenation(unsigned int amount) { indentation += amount; }
    void DecreaseIndentation(unsigned int amount) { indentation -= amount; }
    // TODO(Noah): Make this take in a char *fmt string and ... variadic arguments.
    void write(char *str) {
        int n = 0 = 0;
        std::string currentWrite = "";        
        while (n < strlen(str)) {
            char c = str[n];
            if (c == '\n') {
                fprintf(handle, "%s", currentWrite.c_str());
                currentWrite = "";
                fprintf(handle, "\n");
                freshNewline = true;
            } else {
                if (freshNewline) {
                    char *sillyWhitespace = sillyWhitespace(indentation);
                    fprintf(handle, "%s", sillyWhitespace);
                }
                currentWrite += c;
                freshNewline = false;
            }
            n += 1;
        }
        if (currentWrite != "") {
            fprintf(handle, "%s", currentWrite.c_str());
        }
    }   
};

void GenerateProgram(struct tree_node ast, SpecialFilehandle &sFile);

// NOTE(Noah): Codegen assumes that the AST being pased has GNODE:program
// at the root.
// NOTE(Noah): It also assumes that semantic parsing has been done.
void RunCodegen(struct tree_node ast, char *outFile, enum target_platform target) {
    FILE *fileHandle = fopen(outFile, "w");
    SpecialFilehandle sFileHandle = SpecialFilehandle(fileHandle);
    GenerateProgram(ast, sFileHandle);
}

void GenerateFunction(struct tree_node ast, SpecialFilehandle &sFile);
void GenerateVarDecl(struct tree_node ast, SpecialFilehandle &sFile);
void GenerateStructDecl(struct tree_node ast, SpecialFilehandle &sFile);

// r"[(function)((var_decl);)(struct_decl)]*"
void GenerateProgram(struct tree_node ast, SpecialFilehandle &sFile) {
    for (int i = 0; i < ast.childrenCount; i++) {
        struct tree_node gnode = ast.children[i];
        switch(gnode.metadata.str[0]) {
            case "f": // function
            // TODO(Noah): As I write this, I wonder if this is even
            // a "sensible" way to program this.
            // 1: will this type of high-level traversal through the AST be
            //     "good enough" to produce the underlying LLVM IR?
            // 2: what happens when I want to introduce my package management system
            // or that I want to get away with having function_decl at the top scope
            // of the program be orderless.
            //     at the top of the file, generate function_decl's for all symbols.
            //     this works if we do ASM anyways, because functions are just labels.
            //     this "top of file" step can even be done during AST generation.
            //     i.e., we notice a function, and create a "context" alongside the AST.
            GenerateFunction(gnode, sFile);
            break;
            case "v": // var_decl
            GenerateVarDecl(gnode, sFile);
            sFile.write(";\n");
            break;
            case "s": // struct_decl

            // NOTE(Noah): Now. Let's hold on here. What happens when we want to define a struct
            // local to some scope? How does that even work??
                // For at least a C backend:
                    // put all struct decl's at top level scope of file.
                    // name mangle them for the specific scope.
                    // and do semantic checking to ensure structs are only accessed in the
                    // scope that they were defined.
                // For LLVM IR backend:
                    // structs are now just a high-level concept that we have to deal with internally,
                    // so it is the same general process as the C one, except we actually get more freedom.
            GenerateStructDecl(gnode, sFile);
            break;
        }
    }
}

void GenerateType(struct tree_node ast, SpecialFilehandle &sFile);
void GenerateLv(struct tree_node ast, SpecialFilehandle &sFile);
void GenerateStatement(struct tree_node ast, SpecialFilehandle &sFile);
void GenerateBlock(struct tree_node ast, SpecialFilehandle &sFile);

// r"(type)(symbol)\((lv)(,(lv))*\)[;(statement)]"
void GenerateFunction(struct tree_node ast, SpecialFilehandle &sFile);
    
    // if we were trying to make a new API here, it would be something like the following.
    /* 
        A = GNODE: type
        B = primitive TREE_AST_SYMBOL
        C = list of LV GNODEs
        D? = statement | block
    */
    
    struct tree_node A = ASTGetChildA(ast);
    struct tree_node B = ASTGetChildB(ast);
    struct tree_node C = ASTGetChildC(ast);
    struct tree_node D = ASTGetChildD(ast);

// NOTE(Noah): Below code aint gonna run. Doesn't even make sense. Just trying to figure out
// how this PPL should go about generating code for different platforms :)
#ifdef C_BACKEND

    GenerateType(A, sFile);
    sFile.write( B.metadata.str );
    sFile.write('(');

    for (int i = 0; i < C.childrenCount; i++) {
        GenerateLv(C.children[i], sFile);
        if (i < C.childrenCount - 1) {
            sFile.write(", ");
        }
    }

    sFile.write(")");
    if (D.type != TREE_ROOT) {
        sFile.write("\n");
        switch(D.metadata.str[0]) {
            case "s": // statement
            sFile.write("{\n");
            GenerateStatement(D, sFile);
            sFile.write("}\n");
            break;
            case "b": // block
            GenerateBlock(D, sFile);
            break;
        }
    } else {
        sFile.write(";\n");
    }

#endif
#ifdef LLVM_IR_BACKEND

    // NOTE(Noah): So far, LLVM IR is SUPER EASY.
    sFile.write("define ccc");
    GenerateType(A, sFile);
    sFile.write("@%s", B.metadata.str );
    sFile.write('(');

    for (int i = 0; i < C.childrenCount; i++) {
        GenerateLv(C.children[i], sFile);
        if (i < C.childrenCount - 1) {
            sFile.write(", ");
        }
    }

    sFile.write(")");
    if (D.type != TREE_ROOT) {
        sFile.write("\n");
        switch(D.metadata.str[0]) {
            case "s": // statement
            sFile.write("{\n");
            GenerateStatement(D, sFile);
            sFile.write("}\n");
            break;
            case "b": // block
            GenerateBlock(D, sFile);
            break;
        }
    } else {
        sFile.write(";\n");
    }

#endif

    



#endif