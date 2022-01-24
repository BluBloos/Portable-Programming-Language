/* 
    Inspired by the Essence OS project and their build system, decided PPL should have a similar thing
    going for it. 
    https://nakst.gitlab.io/essence
    NOTE: Many things I copy-and-pasted! :)
*/

// PREVIOUS SCRIPT.
//pushd bin
//g++ -g ../src/ppl.cpp -I ../src/ -o ppl
//g++ -g ../src/tests.cpp -I  ../src/ -o tests
//popd

// TODO(Noah): Deprecate tests.cpp by including them as direct commands in the interactive build system.

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ColorError "\033[0;33m"
#define ColorHighlight "\033[0;36m"
#define ColorNormal "\033[0m"
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

/* ------- TESTS.CPP ------- */
// Standard for any compilation unit of this project.
#include <ppl.h>
enum ppl_test {
    // everything is a single test (because we invoke tests.exe many times with different parameters).
    PTEST_PREPARSER = 0,
    PTEST_INTEGRATION,
    PTEST_REGEX_GEN,
    PTEST_GRAMMER,
    PTEST_COUNT, // comes after all tests
    PTEST_ALL, // comes after count
    PTEST_PREPARSER_ALL,
    PTEST_GRAMMER_ALL
};
void ptest_Preparser(char *inFilePath, int &errors);
void ptest_Grammer(char *inFilePath, int&errors);
/* ------- TESTS.CPP ------- */

void DoCommand(const char *l);
void RemoveEndline(char* l) {
    for (char *pStr = l; *pStr != 0; pStr++) {
        if (*pStr == '\n') 
            *pStr = 0;
    }
}

int main(int argc, char **argv) {
    char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));

    if (strchr(cwd, ' ')) {
		printf("Error: The path to your PPL directory, '%s', contains spaces.\n", cwd);
		return 1;
	}

    printf("Enter 'help' to get a list of commands.\n");

    
    while (1) {
		char *l = NULL;
		size_t pos = 0;
		printf("\n> ");
		printf(ColorHighlight);
		getline(&l, &pos, stdin);
		printf(ColorNormal);
		fflush(stdout);
        RemoveEndline(l);
        DoCommand(l);
	}

}

int CallSystem(const char *buffer) {
	return system(buffer);
}

void DoCommand(const char *l) {

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
        //pushd bin
        //g++ -g ../src/ppl.cpp -I ../src/ -o ppl
        //g++ -g ../src/tests.cpp -I  ../src/ -o tests
        //popd
        CallSystem("g++ -std=c++11 -g src/ppl.cpp -I src/ -o bin/ppl -Wno-writable-strings");

	} else if (0  == strcmp(l, "p") || 0 ==strcmp(l, "preparser")) {
        printf("please enter inFile: ");
        char *inFile = NULL;
		size_t pos = 0;
		printf(ColorHighlight);
		getline(&inFile, &pos, stdin);
		printf(ColorNormal);
		fflush(stdout);
        RemoveEndline(inFile);
        char *inFilePath = SillyStringFmt("tests/preparse/%s", inFile);
        Timer timer = Timer("preparser");
        LOGGER.InitFileLogging("w");
        int errors = 0;
        ptest_Preparser(inFilePath, errors);
        timer.TimerEnd();
    } else if (0 == strcmp(l, "pall") || 0 == strcmp(l, "preparser_all")) {
        
    } else if (0 == strcmp(l, "re") || 0 == strcmp(l, "regex_gen")) {
        //
        printf("re\n");
    } else if (0 == strcmp(l, "g") || 0 == strcmp(l, "grammer")) {
        //
        printf("g\n");
    } else if (0 == strcmp(l, "gall") || 0 == strcmp(l, "grammer_all")) {
        //
        printf("gall\n");
    } else if (0 == strcmp(l, "exit")) {
        exit(0);
    } else if (0 == strcmp(l, "h") || 0 == strcmp(l, "help")) {
        printf(ColorHighlight "\n=== Common Commands ===\n" ColorNormal);
		printf("build           (b)                 - Build.\n");
		printf("preparser       (p)                 - Test preparser on single unit.\n");
		printf("preparser_all   (pall)              - Test preparser on all units in tests/preparse/.\n");
		printf("regex_gen       (re)                - Test LoadGrammer() for building custom regex trees.\n");
		printf("grammer         (g)                 - Test AST generation for a single GNODE on a single unit.\n");
        printf("grammer_all     (gall)              - Test AST generation for all units in tests/grammer/.\n");
		printf("exit                                - Exit the build system.\n");
    } else {
        printf("Unrecognised command '%s'. Enter 'help' to get a list of commands.\n", l);
    }
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
