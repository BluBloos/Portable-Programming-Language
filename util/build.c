/* 
    Inspired by the Essence OS project and their build system, decided PPL should have a similar thing
    going for it. 
    https://nakst.gitlab.io/essence
    NOTE: Many things I copy-and-pasted! :)
*/

/* HOW PPL PROGRAMS ARE BUILT

We have the PPL source file.
|
|
| platform independent step
|
|
pasm.
|
|
| many versions / target machine code
| but independent of platform. 
|
|
Text representation of machine code.
|
|
| platform dependent code!
|
|
binary/bundle for target platform. 

What constitutes the platform dependent code?
- Custom standard library: we can make the standard lib
  do different things depending on the platform.
- Custom entry and exit into program.
- Custom commands we can run on host machine for general file manipulation (useful
    when we will need to create bundles)
    
HOW PPL PROGRAMS ARE BUILT */

// TODO(Noah): For the windows platform, the interactive build system seems to be delayed when you enter commands...

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

/* ------- TESTS.CPP ------- */
// Standard for any compilation unit of this project.
// NOTE(Noah): ppl.h on Windows is the parallel platforms library....I HATE EVERYTHING.
#include <ppl_core.h>
void ptest_Preparser(char *inFilePath, int &errors);
void ptest_Grammer(char *inFilePath, int &errors);
void ptest_wax64(char *inFilePath, int &errors);
/* ------- TESTS.CPaP ------- */

void CheckErrors(int errors) {
    if (errors > 0)
        LOGGER.Error("Completed with %d error(s)", errors);
    else
        LOGGER.Success("Completed with 0 errors.");
}

void PrintHelp();
int DoCommand(const char *l, const char *l2);

// usage ./build [options]
int main(int argc, char **argv) {
    char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));

    if (strchr(cwd, ' ')) {
		printf("Error: The path to your PPL directory, '%s', contains spaces.\n", cwd);
		return 1;
	}

    #ifdef PLATFORM_WINDOWS 
    {
        HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD stdout_mode;
        GetConsoleMode(stdout_handle, &stdout_mode);
        // NOTE(Noah): See https://docs.microsoft.com/en-us/windows/console/setconsolemode for 
        // meaning of 0x0004. The macro was not being resolved...
        SetConsoleMode(stdout_handle, stdout_mode | 0x0004);
    }
    #endif

    if (argc > 1) {
        char *l = argv[1];
        if (argc > 2) {
            char *l2 = argv[2];
            return DoCommand(l, l2);
        }
        return DoCommand(l, NULL);
    } else {
        // Go into interactive mode.
        PrintHelp();
        while (1) {
            char *l = NULL;
            size_t pos = 0;
            printf("\n> ");
            printf(ColorHighlight);
            getline(&l, &pos, stdin);
            printf(ColorNormal);
            fflush(stdout);
            SillyStringRemove0xA(l);
            DoCommand(l, NULL);
            free(l); // a call to getline, if given l=NULL, will alloc a buffer. So we must free it.
        }
    }

    return 1; // should never get here, so return error code.

}

int CallSystem(const char *buffer) {
	return system(buffer);
}

char *GetInFile() {
    printf("please enter inFile: ");
    char *inFile = NULL;
    size_t pos = 0;
    printf(ColorHighlight);
    getline(&inFile, &pos, stdin);
    printf(ColorNormal);
    fflush(stdout);
    SillyStringRemove0xA(inFile);
    return inFile;
}

void PrintHelp() {
    printf("\n");
    printf(ColorHighlight "=== Common Commands ===\n" ColorNormal);
    printf("build           (b)               - Build all cli tools.\n");
    printf("exit                              - Exit the build system.\n");
    printf("\n");
    printf(ColorHighlight "===  PASM  Commands ===\n" ColorNormal);
    printf("pasm_x86_64     (ax64)            - pasm integration test of a single unit for x86_64 target (macOS).\n");
    printf("win_x86_64      (wax64)           - pasm integration test of of a single unit for x86_64 target (Windows).\n");
    printf("win_x86_64_all  (wax64all)        - pasm integration test of all units for x86_64 target (Windows).\n");
    printf("asmparse        (ap)              - Test pplasm parsing capability.\n");
    printf("\n");
    printf(ColorHighlight "===  PPL   Commands ===\n" ColorNormal);
    printf("preparser       (p)               - Test preparser on single unit.\n");
    printf("preparser_all   (pall)            - Test preparser on all units in tests/preparse/.\n");
    printf("regex_gen       (re)              - Test LoadGrammer() for building custom regex trees.\n");
    printf("grammer         (g)               - Test AST generation for a single GNODE on a single unit.\n");
    printf("grammer_all     (gall)            - Test AST generation for all units in tests/grammer/.\n");
}

// Does command then returns the result code.
// anything non-zero is an error.
int DoCommand(const char *l, const char *l2) {

    /* 
        Basically, what different commands do we want to be able to execute in this build system?

        We could have a basic "build" option to just chuck some binaries for PPL in the bin directory.
        I then want all of my different testing things (we basically want to make tests.cpp obsolete).
            - "preparser" or "p"
            - "preparser_all" or "pall"
            - "regex_gen" or "re"
            - "grammer" or "g"
            - "grammer_all" or "gall"
    */

	while ( (l) && (*l == ' ' || *l == '\t')) {
        l++;
    }

	if (0 == strcmp(l, "b") || 0 == strcmp(l, "build")) {
        
        int r = CallSystem("g++ -std=c++11 -g src/ppl.cpp -I vendor/ -I src/ -I backend/src/ -o bin/ppl -Wno-writable-strings -Wno-write-strings");
        if (r == 0) {
            printf("PPL compiler built to bin/ppl\n");
            printf("Usage: ppl <inFile> -o <outFile> -t <TARGET> [options]\n");
        }

        r = CallSystem("g++ -std=c++11 -g backend/src/assembler.cpp -I vendor/ -I src/ -I backend/src/ -o bin/pplasm -Wno-writable-strings \
            -Wno-write-strings");
        if (r == 0) {
            printf("PPL assembler built to bin/pplasm\n");
            printf("Usage: pplasm <inFile> <TARGET>\n");
        }

        return r;

	} else if (0  == strcmp(l, "ap") || 0 ==strcmp(l, "asmparse")) {

        printf("NOTE: cwd is set to backend/tests/\n");
        char *inFile = GetInFile();
        char *inFilePath = SillyStringFmt("backend/tests/%s", inFile);
        Timer timer = Timer("asmparse");
        int errors = 0;
        int r = passembler(inFilePath, "macOS");
        DeallocPasm();
        errors = (r != 0 );
        CheckErrors(errors);
        timer.TimerEnd();
        return (errors > 0);

    } else if (0  == strcmp(l, "ax64") || 0 ==strcmp(l, "pasm_x86_64")) {

        // TODO(Noah): 
        // 1) Mimic pstdlib directory structure changes as they have been made for
        // the Windows standard library.
        // 2) Add stub because the x86_64 backend expects there to be one.
        // 3) Extend macOS pstdlib to have a variadic print.
        // 4) add ax64all command for runnnig all backend tests in Github workflow.
        // 
        // Finally, once all these changes are made, we can move towards writing a proper
        // codegen layer to compile the dynamic array example as seen in the repo README. 

        printf("NOTE: cwd is set to backend/tests/\n");
        char *inFile = GetInFile();
        char *inFilePath = SillyStringFmt("backend/tests/%s", inFile);
        Timer timer = Timer("pasm");
        int errors = 0;
        int r = passembler(inFilePath, "macOS");
        r |= pasm_x86_64(pasm_lines, "bin/out.x86_64", MAC_OS);
        DeallocPasm();
        r |= CallSystem("nasm -f macho64 bin/out.x86_64");
        // TODO(Noah): Implement linking with more libraries.
        r |= CallSystem("nasm -o bin/exit.o -f macho64 backend/pstdlib/macOS/console/exit.s");
        r |= CallSystem("nasm -o bin/print.o -f macho64 backend/pstdlib/macOS/console/print.s");
        r |= CallSystem("ld -o bin/out -static bin/out.o bin/exit.o bin/print.o");
        r |= CallSystem("bin/out");
        errors = (r != 0 );
        CheckErrors(errors);
        timer.TimerEnd();
        return (errors > 0);

    } else if (0  == strcmp(l, "wax64") || 0 ==strcmp(l, "win_x86_64")) {

        printf("NOTE: cwd is set to backend/tests/\n");
        char *inFile;
        if (l2 == NULL) {
            inFile = GetInFile();
        } else {
            inFile = (char *)l2;
        }
        char *inFilePath = SillyStringFmt("backend/tests/%s", inFile);
        Timer timer = Timer("pasm");
        int errors = 0;
        ptest_wax64(inFilePath, errors);
        timer.TimerEnd();
        return (errors > 0);
    
    } else if (0  == strcmp(l, "wax64all") || 0 ==strcmp(l, "win_x86_64_all")) {

        Timer timer = Timer("win_x86_64_all");
        LOGGER.InitFileLogging("w");
        int errors = 0;
        DIR *dir;
        struct dirent *ent;
        char *dirName = "backend/tests";
        if ((dir = opendir(dirName)) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                if (ent->d_name[0] != '.') {
                    char *pStr;
                    for (pStr = ent->d_name; *pStr != 0 && *pStr != '.'; pStr++);
                    pStr++; // skp past the '.'
                    if (SillyStringEquals("pasm", pStr)) {
                        ptest_wax64(SillyStringFmt("%s/%s", dirName, ent->d_name), errors);
                    }
                }
            }
            closedir(dir);
        } else {
            LOGGER.Error("Unable to open %s", dirName);
            errors += 1;
        }
        timer.TimerEnd();
        return (errors > 0);

    } else if (0  == strcmp(l, "p") || 0 ==strcmp(l, "preparser")) {
        
        printf("NOTE: cwd is set to tests/preparse/\n");
        char *inFile = GetInFile();
        char *inFilePath = SillyStringFmt("tests/preparse/%s", inFile);
        Timer timer = Timer("preparser");
        LOGGER.InitFileLogging("w");
        int errors = 0;
        ptest_Preparser(inFilePath, errors);
        CheckErrors(errors);
        timer.TimerEnd();
        return (errors > 0);

    } else if (0 == strcmp(l, "pall") || 0 == strcmp(l, "preparser_all")) {
        
        Timer timer = Timer("preparser_all");
        LOGGER.InitFileLogging("w");
        int errors = 0;
        DIR *dir;
        struct dirent *ent;
        char *dirName = "tests/preparse";
        if ((dir = opendir (dirName)) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                if (ent->d_name[0] != '.') {
                    ptest_Preparser( SillyStringFmt("%s/%s", dirName, ent->d_name), errors);
                }
            }
            closedir (dir);
        } else {
            LOGGER.Error("Unable to open %s", dirName);
            errors += 1;
        }
        CheckErrors(errors);
        timer.TimerEnd();
        return (errors > 0);

    } else if (0 == strcmp(l, "re") || 0 == strcmp(l, "regex_gen")) {
        
        Timer timer = Timer("regex_gen");
        LOGGER.InitFileLogging("w");
        int errors = 0;
        LoadGrammer();
        CheckErrors(errors);
        timer.TimerEnd();
        return (errors > 0);

    } else if (0 == strcmp(l, "g") || 0 == strcmp(l, "grammer")) {
        
        printf("NOTE: cwd is set to tests/grammer/\n");
        char *inFile = GetInFile();
        Timer timer = Timer("grammer");
        LOGGER.InitFileLogging("w");
        int errors = 0;
        LoadGrammer();
        char *inFilePath = SillyStringFmt("tests/grammer/%s", inFile);
        ptest_Grammer(inFilePath, errors);
        CheckErrors(errors);
        timer.TimerEnd();
        return (errors > 0);

    } else if (0 == strcmp(l, "gall") || 0 == strcmp(l, "grammer_all")) {
        
        Timer timer = Timer("grammer_all");
        LOGGER.InitFileLogging("w");
        int errors = 0;
        // TODO(Noah): Abstract this stuff. This sort of directory read all files thing
        // it's common between many different tests that I might want to do.
        LoadGrammer();
        DIR *dir;
        struct dirent *ent;
        char *dirName = "tests/grammer";
        if ((dir = opendir (dirName)) != NULL) {
            /* print all the files and directories within directory */
            while ((ent = readdir (dir)) != NULL) {
                if (ent->d_name[0] != '.') {
                    ptest_Grammer( SillyStringFmt("%s/%s", dirName, ent->d_name), errors);
                }
            }
            closedir (dir);
        } else {
            LOGGER.Error("Unable to open %s", dirName);
            errors += 1;
        }
        CheckErrors(errors);
        timer.TimerEnd();
        return (errors > 0);

    } else if (0 == strcmp(l, "exit")) {
        exit(0);
    } else if (0 == strcmp(l, "h") || 0 == strcmp(l, "help")) {
        PrintHelp();
        return 0;
    }

    printf("Unrecognised command '%s'. Enter 'help' to get a list of commands.\n", l);
    return 1;
}

void ptest_Grammer(char *inFilePath, int&errors) {
    FILE *inFile = fopen(inFilePath, "r");
    LOGGER.Log("Testing grammer for: %s", inFilePath);
    if (inFile == NULL) {
        LOGGER.Error("inFile of '%s' does not exist", inFilePath);
        errors += 1;
    } else {
        TokenContainer tokensContainer;
        PreparseContext preparseContext;
        if (LexAndPreparse(inFile, tokensContainer, preparseContext)) {
            if (VERBOSE) {
                tokensContainer.Print();
            }

            // Now we try to parse for the grammer object.
            // we know which specific grammer definition via the name of
            // the inFile that was given.

            char grammerDefName[256] = {};
            
            // NOTE(Noah): Alright, so we got some truly dumbo code here :)
            char *onePastLastSlash; 
            for (char *pStr = inFilePath; *pStr != 0; pStr++ ) {
                if (*pStr == '/') {
                    onePastLastSlash = pStr;
                }
            }
            onePastLastSlash++; // get it to one past the last slash.
            
            memcpy( grammerDefName, onePastLastSlash, strlen(onePastLastSlash) - 3 );
            //LOGGER.Log("grammerDefName: %s", grammerDefName);

            struct tree_node tree = {};
            
            bool r = ParseTokensWithGrammer(
                tokensContainer, 
                GRAMMER.GetDef(grammerDefName),
                tree);
            
            //bool r = false;

            if (r) {
                PrintTree(tree, 0);
                DeallocTree(tree);
            }
            else {
                LOGGER.Error("ParseTokensWithGrammer failed.");
                errors += 1;
            } 

        } else {
            LOGGER.Error("LexAndPreparse failed.");
            errors += 1;
        }
    }
    fclose(inFile);
}

void ptest_Preparser(char *inFilePath, int &errors) {
    FILE *inFile = fopen(inFilePath, "r");
    LOGGER.Log("Testing parser for: %s", inFilePath);
    if (inFile == NULL) {
        LOGGER.Error("inFile of '%s' does not exist", inFilePath);
        errors += 1;
    } else {
        TokenContainer tokensContainer;
        PreparseContext preparseContext;
        if (LexAndPreparse(inFile, tokensContainer, preparseContext)) {
            if (VERBOSE) {
                tokensContainer.Print();
            }
        } else {
            LOGGER.Error("LexAndPreparse failed.");
            errors += 1;
        }
    }
    fclose(inFile);
}

void ptest_wax64(char *inFilePath, int &errors) {
    LOGGER.Log("Testing assembler for: %s", inFilePath);
    int r = passembler(inFilePath, "macOS"); // TODO(Noah): target independent, remove macOS.
    r = pasm_x86_64(pasm_lines, "bin/out.x86_64", MAC_OS); // TODO(Noah): target independent, remove macOS.
    DeallocPasm();
    r = CallSystem("nasm -g -o bin\\out.obj -f win64 bin/out.x86_64");
    r = CallSystem("nasm -g -o bin\\exit.obj -f win64 backend/pstdlib/Windows/exit.s");
    r = CallSystem("nasm -g -o bin\\stub.obj -f win64 backend/pstdlib/Windows/stub.s");
    r = CallSystem("nasm -g -o bin\\print.obj -f win64 backend/pstdlib/Windows/console/print.s");
    // TODO(Noah): Remove dependency on Visual Studio linker.
    r = CallSystem("link /LARGEADDRESSAWARE /subsystem:console /entry:start bin/out.obj bin/exit.obj bin/stub.obj bin/print.obj \
        /OUT:bin/out.exe Kernel32.lib");
    r = CallSystem("bin\\out.exe");
    errors = (r != 0 );
    if (errors > 0) {
        LOGGER.Error("Completed with %d error(s)", errors);
        LOGGER.Error("Return code: %d", r);
    } else {
        LOGGER.Success("Completed with 0 errors.");
    }
}
