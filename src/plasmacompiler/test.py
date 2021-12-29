# TEST DRIVEN DEVELOPMENT

import os
import subprocess
import compiler
from colorama import init
from colorama import Fore, Back, Style
init()

def colored(string, color):
    if color == "green":
        return Fore.GREEN + string
    elif color == "red":
        return Fore.RED + string

PLATFORM = "WINDOWS"

def SingleTest(fileName, desired_result):
    debug = False

    test = True

    result = compiler.Run(fileName, debug, test, PLATFORM)

    if desired_result == result:
        print(colored("{} works.".format(fileName), "green"))
        print(Style.RESET_ALL)
    else:
        print(colored("Error: {} does not work.".format(fileName), "red"))
        print(Style.RESET_ALL)

SingleTest("tests/variable_scoping.c", 3)
SingleTest("tests/variables.c", 5)
SingleTest("tests/expression.c", 1)
SingleTest("tests/comments.c", 0)
SingleTest("tests/function.c", 65)
SingleTest("tests/function2.c", 128)
SingleTest("tests/factorial.c", 6)
SingleTest("tests/if.c", 100)
SingleTest("tests/if2.c", 80)
SingleTest("tests/if3.c", 40)
SingleTest("tests/conditional.c", 12)
SingleTest("tests/fib.c", 13)
SingleTest("tests/for.c", 5)
SingleTest("tests/while.c", 10)
