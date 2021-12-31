import codegen

# TODO(Noah): Change compiler to simply go from lexer (tokens) right to syntax tree, then output step before the linker.
def Run(ast, outFile, logger):
    # Object file name
    obj_filePath = outFile + ".cpp"
    codegen.Run(ast, obj_filePath, logger)
    # TODO(Noah): How do we know if this has failed??
    # But it should not actually fail. It's determiniatic. The AST to code is known well in advance.
    return obj_filePath, True
