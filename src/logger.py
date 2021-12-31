from colorama import init
from colorama import Fore, Back, Style

recordingActive = False
recordingSave = []
init()

def colored(string, color):
    if color == "green":
        return Fore.GREEN + string
    elif color == "red":
        return Fore.RED + string

class Logger:
    def __init__(self):
        self.file_handle = open("log.txt", "w")
    def __del__(self):
        self.file_handle.close()
    def printf(self, string):
        self.file_handle.write(string)
        self.file_handle.write('\n')
    def Log(self, string):
        _str = "[LOG]: " + string
        # Print to File.
        self.printf(_str)
        #PrintF(_str)
        print(_str) # No need for color information.
    def Error(self, string):
        _str = "[ERROR]: " + string
        self.printf(_str)
        # Print to console with red color.
        print(colored(_str, "red"))
        print(Style.RESET_ALL, end='')
    def Success(self, string):
        _str = "[SUCCESS]: " + string
        self.printf(_str)
        # Print to console with green color.
        print(colored(_str, "green"))
        print(Style.RESET_ALL, end='')




'''
def StartInternalRecording():
    global recordingActive
    recordingActive = True

def StopInternalRecording():
    global recordingActive
    recordingActive = False

def ClearRecording():
    global recordingSave
    recordingSave = []

# NOTE(Noah): Kind of flushes the first...
def FlushLast():
    global file_handle
    global recordingSave
    if len(recordingSave):
        PrintF(recordingSave[0])
'''