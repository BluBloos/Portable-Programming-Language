from subprocess import call
import os
import time


if os.path.isfile("test.exe"):
    BEGIN = time.time()
    print(call("test.exe"))
    END = time.time() - BEGIN
    print("ELAPSED: " + str(END * 1000) + "ms")
