#ifndef CODEGEN_H
#define CODEGEN_H

// Replace SpecialFilehandle with PFileWriter

void GenerateProgram(struct tree_node ast, PFileWriter &fileWriter);

// NOTE(Noah): Codegen assumes that the AST being pased has GNODE:program
// at the root.
// NOTE(Noah): It also assumes that semantic parsing has been done.
//  NOTE(Noah): ^ this is a big note.
void RunCodegen(struct tree_node ast, char *outFile) {
    FILE *fileHandle = fopen(outFile, "w");
    PFileWriter fileWriter = SpecialFilehandle(fileHandle);
    GenerateProgram(ast, fileWriter);
}

#endif