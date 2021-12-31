file_handle = None
recordingActive = False
recordingSave = []

def StartInternalRecording():
    global recordingActive
    recordingActive = True

def StopInternalRecording():
    global recordingActive
    recordingActive = False

def ClearRecording():
    global recordingSave
    recordingSave = []

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

# NOTE(Noah): Kind of flushes the first...
def FlushLast():
    global file_handle
    global recordingSave
    if len(recordingSave):
        PrintF(recordingSave[0])

def Error(string):
    global recordingActive
    global recordingSave
    _str = "[ERROR]: " + string
    if recordingActive:
        recordingSave.append(_str)
    else:
        PrintF(_str)
        print(_str)

def Log(string):
    global recordingActive
    global recordingSave
    _str = "[LOG]: " + string
    if recordingActive:
        recordingSave.append(_str)
    else:
        PrintF(_str)
        print(_str)

'''
def Error(string):
    print("[ERROR]: " + string)

def Log(string):
    print("[LOG]: " + string)
'''

