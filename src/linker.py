import os
# TODO(Noah): This part is likely where are all the 
def Run(objectFilePath, outFile):
    # - filePath is the previously generated file from the codegen step.
    # Now we "link" this file.
    # - pContext is the preparser context -> tells us what standard libraries to include and stuff.

    # NOTE(Noah): Need to make this more robust and so forth...
    os.system("g++ {} -o {}".format(objectFilePath, outFile))