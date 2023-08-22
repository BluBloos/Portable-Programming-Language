#ifndef CODEGEN_H
#define CODEGEN_H

// Replace SpecialFilehandle with PFileWriter

void GenerateProgram(struct tree_node ast, PFileWriter &fileWriter);

// NOTE(Noah): Codegen assumes that the AST being passed has GNODE:program
// at the root.
// NOTE(Noah): It also assumes that semantic parsing has been done.
//  NOTE(Noah): ^ this is a big note.
void RunCodegen(struct tree_node ast, const char *outFile) {
    FILE *fileHandle = fopen(outFile, "w");
    PFileWriter fileWriter = PFileWriter(fileHandle); // takes ownership of file handle.
    GenerateProgram(ast, fileWriter);
}

// NOTE: the goal of generate program should be to execute PASM assembly instructions.
// we'll then run those through our assembler to generate the actual .EXE 
void GenerateProgram(struct tree_node ast, PFileWriter &fileWriter)
{
    // ast node is "[(function)((var_decl);)]*"

    // TODO: maybe we want to use the iterator pattern. that might make things nice.
    for ( uint32 i = 0; i < ast.childrenCount; i++)
    {
        tree_node child = ast.children[i];
        if (child.type == TREE_AST_GNODE)
        {
            // TODO: please almighty god, let's get rid of these strings.
            if (0 == strcmp(child.metadata.str, "function")) {
                fileWriter.write("function\n");
            }
            else if (0 == strcmp(child.metadata.str, "var_decl"))
            {
                fileWriter.write("var_decl\n");
            }
        }
    }
}

#endif