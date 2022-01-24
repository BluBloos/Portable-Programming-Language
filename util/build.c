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

#include <stdio.h>
#include <string.h>

#define ColorError "\033[0;33m"
#define ColorHighlight "\033[0;36m"
#define ColorNormal "\033[0m"
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

void DoCommand(const char *l);

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

        // TODO(Noah): Get rid of the \n at the end of the line before passing to DoCommand.
        for (char *pStr = l; *pStr != 0; pStr++) {
            if (*pStr == '\n') 
                *pStr = 0;
        } 
		
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
        //
        printf("p\n");
    } else if (0 == strcmp(l, "pall") || 0 == strcmp(l, "preparser_all")) {
        //
        printf("pall\n");
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

