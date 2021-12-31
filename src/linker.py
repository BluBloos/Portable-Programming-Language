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
    # NOTE(Noah): pContext only keeps include directives that are either ppl standard lib or C standard lib.
    for lib in pContext.libs:
        if not lib.startswith("ppl"):
            libLines.append("#include <{}>".format(lib))
    InjectLines(objectFilePath, libLines, logger)

    # TODO: I can determine from the preparser step what precompiled .o files to link with (my standard library)

    # NOTE(Noah): Need to make this more robust and so forth...
    os.system("g++ {} -o {}".format(objectFilePath, outFile))