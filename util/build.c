/* 
    Inspired by the Essence OS project and their build system, decided PPL should have a similar thing
    going for it. 
    https://nakst.gitlab.io/essence
    NOTE: Many things I copy-and-pasted! :)
*/


#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

/* ------- TESTS.CPP ------- */
// Standard for any compilation unit of this project.
// NOTE(Noah): ppl.h on Windows is the parallel platforms library....I HATE EVERYTHING.
#include <ppl_core.h>
void ptest_Preparser(char *inFilePath, int &errors);
void ptest_Grammer(char *inFilePath, int&errors);
/* ------- TESTS.CPaP ------- */

void CheckErrors(int errors) {
    if (errors > 0)
        LOGGER.Error("Completed with %d error(s)", errors);
    else
        LOGGER.Success("Completed with 0 errors.");
}

void PrintHelp();
int DoCommand(const char *l);

// usage ./build [options]
int main(int argc, char **argv) {
    char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));

    if (strchr(cwd, ' ')) {
		printf("Error: The path to your PPL directory, '%s', contains spaces.\n", cwd);
		return 1;
	}

    if (argc > 1) {
        char *l = argv[1];
        return DoCommand(l);
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
            DoCommand(l);
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
    printf(ColorHighlight "\n=== Common Commands ===\n" ColorNormal);
    printf("build           (b)                 - Build all cli tools.\n");
    printf("asmhello        (ah)                - Test pplasm assembler on backend/helloworld.\n");
    printf("preparser       (p)                 - Test preparser on single unit.\n");
    printf("preparser_all   (pall)              - Test preparser on all units in tests/preparse/.\n");
    printf("regex_gen       (re)                - Test LoadGrammer() for building custom regex trees.\n");
    printf("grammer         (g)                 - Test AST generation for a single GNODE on a single unit.\n");
    printf("grammer_all     (gall)              - Test AST generation for all units in tests/grammer/.\n");
    printf("exit                                - Exit the build system.\n");
}

// Does command then returns the result code.
// anything non-zero is an error.
int DoCommand(const char *l) {

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
        
        int r = CallSystem("g++ -std=c++11 -g src/ppl.cpp -I src/ -I backend/src/ -o bin/ppl -Wno-writable-strings -Wno-write-strings");
        if (r == 0) {
            printf("PPL compiler built to bin/ppl\n");
            printf("Usage: ppl <inFile> -o <outFile> -t <TARGET> [options]\n");
        }

        r = CallSystem("g++ -std=c++11 -g backend/src/assembler.cpp -I src/ -I backend/src/ -o bin/pplasm -Wno-writable-strings \
            -Wno-write-strings");
        if (r == 0) {
            printf("PPL assembler built to bin/pplasm\n");
            printf("Usage: pplasm <inFile> <TARGET>\n");
        }

        return r;

	} else if (0  == strcmp(l, "ah") || 0 ==strcmp(l, "asmhello")) {

        int r = passembler("backend/helloworld/helloworld.pasm", "macOS");
        return r;

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
            /* print all the files and directories within directory */
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
