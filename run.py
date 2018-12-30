from subprocess import call
import os
if os.path.isfile("test.exe"):
    print(call("test.exe"))
