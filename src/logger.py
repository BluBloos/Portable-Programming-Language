file_handle = None

def InitLogger():
    global file_handle
    file_handle = open("log.txt", "w")

def CloseLogger():
    global file_handle
    file_handle.close()

def PrintF(string):
    global file_handle
    file_handle.write(string)
    file_handle.write('\n')

def Error(string):
    _str = "[ERROR]: " + string
    PrintF(_str)
    print(_str)

def Log(string):
    _str = "[LOG]: " + string
    PrintF(_str)
    print(_str)

'''
def Error(string):
    print("[ERROR]: " + string)

def Log(string):
    print("[LOG]: " + string)
'''

