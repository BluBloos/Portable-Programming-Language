import os

def InjectLines(objectFilePath, lines, logger):
    try:
        file = open(objectFilePath, "r+")
        content = file.read()
        file.seek(0,0)
        for line in lines:
            file.write(line.rstrip('\r\n') + '\n')
        file.write(content)
    except IOError as e:
        logger.Error(str(e))

# TODO(Noah): This part is likely where are all the 
def Run(pContext, objectFilePath, outFile, logger):
    
    # - In the object file I can inject at the top standard C library things, Ex) stdio.h
    libLines = []
    lib_includes = []
    # NOTE(Noah): pContext only keeps include directives that are either ppl standard lib or C standard lib.
    for lib in pContext.libs:
        if not lib.startswith("ppl"):
            libLines.append("#include <{}>".format(lib))
        else:
            lib_inc = lib.replace("/", "_") + ".o"
            lib_includes.append(lib_inc)
            libLines.append("#include <{}>".format(lib_inc.replace(".o", ".h"))) # so we can link.

    # add the core ppl lib
    libLines.append("#include <ppl_core.h>")

    InjectLines(objectFilePath, libLines, logger)

    include_path = "src/std/"
    include_paths_cmd = "-I {}".format(include_path)
    
    # NOTE(Noah): Need to make this more robust and so forth...
    os.system("g++ {} {} -o {} {}".format(objectFilePath, \
        " ".join([ (include_path + lib) for lib in lib_includes]) ,outFile, include_paths_cmd))