/* 
    Inspired by the Essence OS project and their build system, decided PPL should have a similar thing
    going for it. 
    https://nakst.gitlab.io/essence
    NOTE: Many things I copy-and-pasted! :)
*/


// TODO: so for these tests in general we want to not just see that things run to
// completion but rather that the output of the test is correct.
//
// TODO: there is also the task of better timing. right now the timing includes any
// printing. that is non-ideal. I would prefer that we finish timing as soon as the
// actual work is complete.


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
void ptest_Lexer(char *inFilePath, TokenContainer groundTruth, int &errors);
void ptest_Grammar(char *inFilePath, int &errors);
void ptest_Codegen(char *inFilePath, int &errors);
void ptest_Preparser(char *inFilePath, int &errors);
void ptest_wax64(char *inFilePath, int &errors);
int ptest_Lexer_all();
void ptest_ax64(char *inFilePath, int &errors);
int RunPtestFromInFile(void (*ptest)(char *inFilePath, int &errors), const char *testName, const char *cwd);
int RunPtestFromInFile(void (*ptest)(char *inFilePath, int &errors), const char *testName, const char *cwd, const char *inFile);
int RunPtestFromCwd(
    void (*ptest)(char *inFilePath, int &errors), 
    bool (*validate)(const char *fileName),
    const char *testName, 
    const char *cwd
);

int splitCommandLine(char * commandBuffer, char* args[], int maxargs, char delimiter);
/* ------- TESTS.CPaP ------- */

void PrintIfError(int errors) {
    if (errors > 0)
        LOGGER.Error("Completed with %d error(s)", errors);
    else
        LOGGER.Success("Completed with 0 errors.");
}

void PrintHelp();
int DoCommand(char *l, const char *l2);

// usage ./build [options]
int main(int argc, char **argv) {
    
    TimerGlob()->clocksPerSecond = estimate_timer_frequency();

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
        // maybe here is it 4 or something. undefined case.
        return DoCommand(l, (char *)0);
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
            char* args[2];
            int nArgs = splitCommandLine(l, args, 2, ' ');
            if (nArgs == 1)            DoCommand(args[0], 0);
            else if (nArgs == 2)            DoCommand(args[0], args[1]);
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

void PrintAbout()
{
    printf("\n");
    // TODO: re-enable the `build` command. for now, the compiler will actually be via the build.exe;
    // this is laziness at its best!

    printf("Program name:            Portable Programming Language (PPL) Toolsuite\n");
    printf("Version:                 v0.1.0\n");
    printf("How to read help menu:   <fullCmdName> (<shortCmdName>)   - <desc>\n");
    printf("Program dependencies:    Netwide Assembler (nasm) must be available from the cmdline.\n");
    printf("                         link.exe from Visual Studio Community 2022.\n");
}

void PrintHelp() {
    printf("\n");
    printf(ColorHighlight "=== Common Commands ===" ColorNormal "\n");
    // TODO: re-enable the `build` command. for now, the compiler will actually be via the build.exe;
    // this is laziness at its best!
    //printf("build           (b)               - Build all cli tools.\n");
    printf("about           (a)               - Print additional info about this program.\n");
    printf("help            (h)               - Print this help menu.\n");
    printf("exit                              - Exit the build system.\n");
    printf("\n");
#ifndef RELEASE
    printf(ColorHighlight "===  PASM  Commands ===" ColorNormal "\n");
    // TODO(Noah): Better naming scheme for macOS please...
    printf("pasm_x86_64     (ax64)            - pasm integration test of a single unit for x86_64 target (macOS).\n");
    printf("pasm_x86_64_all (ax64all)         - pasm integration test of all units for x86_64 target (macOS).\n");
    printf("win_x86_64      (wax64)           - pasm integration test of of a single unit for x86_64 target (Windows).\n");
    printf("win_x86_64_all  (wax64all)        - pasm integration test of all units for x86_64 target (Windows).\n");
    printf("asmparse        (ap)              - Test pasm parsing capability.\n");
    printf("\n");
#endif
    printf(ColorHighlight "===  PPL   Commands ===" ColorNormal "\n");
    
#ifndef RELEASE
    printf("lexer           (l)               - Test lexer on single unit.\n");
    printf("lexer_all       (lall)            - Test lexer on all units in tests/preparse/.\n");
    printf("preparser       (p)               - Test preparser on single unit.\n");
    printf("preparser_all   (pall)            - Test preparser on all units in tests/preparse/.\n");
    printf("regex_gen       (re)              - Test LoadGrammar() for building custom regex trees.\n");
    printf("grammer         (g)               - Test AST generation for a single unit.\n");
    printf("grammer_all     (gall)            - Test AST generation for all units in tests/grammer/.\n");
    printf("\n");
    printf("codegen         (c)               - Test Codegen for a single file.\n");
#endif
    printf("compile         (cl)              - Compile a single .PPL file to the target's executable format.\n");
    printf("run             (r)               - Run the most recently compiled .PPL file.\n");
}

// TODO: this idea is actually something that should be impl in nc::pal.
// but it should only be so if we can do this at compile-time.
#include <algorithm>
std::string ModifyPathForPlatform(const char *filePath)
{
    // I purposefully did the most stupid thing here.
    std::string result = std::string(filePath);

#if defined(_MSC_VER)
    std::replace(result.begin(), result.end(), '/', '\\' );
#endif

    return result;
}

// Does command then returns the result code.
// anything non-zero is an error.
int DoCommand( char *l, const char *l2) {

#define BACKEND_TESTS_DIR "tests/backend"
#define PSTDLIB_WINDOWS_DIR "pstdlib"
#define PSTDLIB_UNIX_DIR "pstdlib"

    // Remove leading whitespace.
	while ( (l) && (*l == ' ' || *l == '\t')) {
        l++;
    }

	// TODO: I'm not even sure if this is going to work on Windows.
    
    if (0 == strcmp(l, "exit")) {
        
        exit(0);

    }
#ifndef RELEASE
    else if (0 == strcmp(l, "b") || 0 == strcmp(l, "build")) {
        
        int r = CallSystem(
            ModifyPathForPlatform(
                "g++ -std=c++11 -g src/ppl.cpp -I vendor/ -I src/ -o bin/ppl -Wno-writable-strings -Wno-write-strings").c_str()
        );
        if (r == 0) {
            printf("PPL compiler built to bin/ppl\n");
            printf("Usage: ppl <inFile> -o <outFile> -t <TARGET> [options]\n");
        }

        r = CallSystem(
            ModifyPathForPlatform("g++ -std=c++11 -g src/assembler.cpp -I vendor/ -I src/ -o bin/pplasm -Wno-writable-strings \
            -Wno-write-strings").c_str() );
        if (r == 0) {
            printf("PPL assembler built to bin/pplasm\n");
            printf("Usage: pplasm <inFile> <TARGET>\n");
        }

        return r;

	} else if (0  == strcmp(l, "ap") || 0 ==strcmp(l, "asmparse")) {

        // TODO(Noah): Can be modularized via some variant on RunPtest...
        printf("NOTE: cwd is set to " BACKEND_TESTS_DIR "/\n");
        char *inFile = GetInFile();
        char *inFilePath = SillyStringFmt( ModifyPathForPlatform( BACKEND_TESTS_DIR "/%s").c_str() , inFile);
        Timer timer = Timer("asmparse");
        int errors = 0;
        int r = passembler(inFilePath, "macOS");
        DeallocPasm();
        errors = (r != 0 );
        PrintIfError(errors);
        timer.TimerEnd();
        return (errors > 0);

    } 
    else if (0 == strcmp(l, "gall") || 0 == strcmp(l, "grammer_all")) {
        
        LoadGrammar();
        printf(
            "\nPlease note that grammar tests use the filename to derive the grammar\n"
              "production to test.\n");
        return RunPtestFromCwd(
            ptest_Grammar,
            // TODO(Noah): Maybe we abstract the lambda here beause we use the same
            // lambda twice?
            [](const char *fileName) -> bool {
                return (fileName[0] != '.');
            },
            "grammar_all", 
            ModifyPathForPlatform("tests/grammar").c_str()
        );

    }
    else if (0  == strcmp(l, "ax64") || 0 ==strcmp(l, "pasm_x86_64")) {

        return RunPtestFromInFile(ptest_ax64, "pasm", ModifyPathForPlatform( BACKEND_TESTS_DIR "/").c_str() );

    } else if (0  == strcmp(l, "ax64all") || 0 ==strcmp(l, "pasm_x86_64_all")) {

        // TODO(Noah): Once more we have a case where we can abstract things because there is hella
        // cmd+c cmd+v going on.
        // The code we have below is quite literally the same as we see for the wax64all case,
        // except we are calling ptest_ax64 instead...

        return RunPtestFromCwd(
            ptest_ax64,
            [](const char *fileName) -> bool {
                if (fileName[0] != '.') {
                    const char *pStr;
                    for (pStr = fileName; *pStr != 0 && *pStr != '.'; pStr++);
                    pStr++; // skp past the '.'
                    if (SillyStringEquals("pasm", pStr)) {
                        return true;
                    }
                }
                return false;
            },
            "pasm_x86_64_all",
            ModifyPathForPlatform(BACKEND_TESTS_DIR).c_str()
        );

    } else if (0  == strcmp(l, "wax64") || 0 ==strcmp(l, "win_x86_64")) {

        if (l2 == 0) {
            return RunPtestFromInFile(ptest_wax64, "pasm", ModifyPathForPlatform(BACKEND_TESTS_DIR "/").c_str() );
        } else {
            return RunPtestFromInFile(ptest_wax64, "pasm", ModifyPathForPlatform(BACKEND_TESTS_DIR "/").c_str(), (char *)l2);
        }
    
    } else if (0  == strcmp(l, "wax64all") || 0 ==strcmp(l, "win_x86_64_all")) {

        return RunPtestFromCwd(
            ptest_wax64, 
            [](const char *fileName) -> bool {
                if (fileName[0] != '.') {
                    const char *pStr;
                    for (pStr = fileName; *pStr != 0 && *pStr != '.'; pStr++);
                    pStr++; // skp past the '.'
                    if (SillyStringEquals("pasm", pStr)) {
                        return true;
                    }
                }
                return false;
            },
            "win_86_64_all",
            ModifyPathForPlatform( BACKEND_TESTS_DIR).c_str()
        );

    } else if (0  == strcmp(l, "l") || 0 ==strcmp(l, "lexer")) {
        
        #if 0
        return RunPtestFromInFile(
            ptest_Lexer, "lexer", ModifyPathForPlatform("tests/preparse/").c_str() );
#else
return 1;
#endif

    } else if (0 == strcmp(l, "lall") || 0 == strcmp(l, "lexer_all")) {

        return ptest_Lexer_all();

    } else if (0  == strcmp(l, "p") || 0 ==strcmp(l, "preparser")) {

        return RunPtestFromInFile(
            ptest_Preparser, "preparser", ModifyPathForPlatform("tests/preparse/").c_str() );

    } else if (0  == strcmp(l, "pall") || 0 ==strcmp(l, "preparser_all")) {

        // TODO: So like, one of the really cool things you can do with a nice compile-time metaprogramming
        // sort of idea is, based on the system that I am compiling on, modify my strings to either do `/` or `\`.
        return RunPtestFromCwd(
            ptest_Preparser,
            [](const char *fileName) -> bool {
                return (fileName[0] != '.');
            },
            "preparser_all",
            ModifyPathForPlatform("tests/preparse").c_str()
        );

    }
    else if (0 == strcmp(l, "re") || 0 == strcmp(l, "regex_gen")) {
        
        Timer timer = Timer("regex_gen");
        LOGGER.InitFileLogging("w");
        int errors = 0;
        LoadGrammar();
        PrintIfError(errors);
        timer.TimerEnd();
        return (errors > 0);

    } else if (0 == strcmp(l, "g") || 0 == strcmp(l, "grammar")) {
        
        LoadGrammar();
        // TODO: this printf is copy-pasta to the one for grammar_all.
        printf(
            "\nPlease note that grammar tests use the filename to derive the grammar\n"
              "production to test.\n");
        return RunPtestFromInFile(ptest_Grammar, "grammar", ModifyPathForPlatform("tests/grammar/").c_str() );

    } else if (0 == strcmp(l , "c") ||  0 == strcmp(l, "codegen")) {

        LoadGrammar();

        return RunPtestFromInFile(
            ptest_Codegen,
            "codegen", 
            "tests/"
        );
    }
#endif
    else if (0 == strcmp(l , "r") ||  0 == strcmp(l, "run")) {
#if defined(PLATFORM_MAC)
        int r = CallSystem("(./myProgram && echo \"return code: 0\") || echo \"return code: $?\"");
#elif defined(PLATFORM_WINDOWS)
        PPL_TODO;
#endif
        return 0;
    }
    // COMPILE COMMAND.
    else if (0 == strcmp(l , "cl") ||  0 == strcmp(l, "compile")) {

        LoadGrammar();
        
        int errors = 0;
        int result = 0;

        char *inFilePath;
        const char *outFilePath = "myProgram.pasm";

        if (l2 == 0) {
            // TODO: issue.
            LOGGER.Error("'cl' command requires a file path argument");
            errors += 1;    
        } else {
            inFilePath = (char *)l2;
        }

#if 0
        // TODO: prob hook with explicit codegen test mode.
        result = RunPtestFromInFile(
            ptest_Codegen,
            "codegen", 
            "tests/"
        );

        if (result != 0) errors += 1;
#else
        if (errors == 0) {
            FILE *inFile = fopen(inFilePath, "r");
            if (inFile == NULL) {
                LOGGER.Error("inFile of '%s' does not exist", inFilePath);
                errors += 1;
            } else {
                TokenContainer tokensContainer;
                RawFileReader tokenBirthplace;
                ppl_error_context bestErr = {};
                if (Lex(inFile, tokensContainer, &tokenBirthplace, &bestErr)) {
                    

                    const char *grammarDefName = "program";
                    
                    struct tree_node tree = {};
                    
                    bool r = ParseTokensWithGrammar(
                        tokensContainer, 
                        GRAMMAR.GetDef(grammarDefName),
                        &tree, bestErr);

                    if (r) {
                
                        if (VERBOSE) PrintTree(tree, 0);

                        CG_Create(); // init the codegen system.
                        
                        RunCodegen(tree, outFilePath);
                        
                        CG_Release(); // shutdown the codegen system.
                        
                        DeallocTree(tree);
                    }
                    else {
                        // emit the best error that we got back.
                        {
                            uint32_t c = bestErr.c;
                            uint32_t line = bestErr.line;

                            // TODO: The below is likely to change when #import actually works.
                            // maybe it comes from the error because the source code that errors
                            // is in diff file.
                            const char *file = LOGGER.logContext.currFile;

                            const char *code = 
                                bestErr.codeContext ? bestErr.codeContext : "<unknown>"; // TODO.

                            LOGGER.EmitUserError(
                                file, line, c, code,
                                bestErr.errMsg ? bestErr.errMsg : "<unknown>" 
                            );

                            if (bestErr.kind == PPL_ERROR_KIND_PARSER)
                            {
                                LOGGER.Min("The almost-parsed AST:\n");
                                LOGGER.Min("%s\n",bestErr.almostParsedTree);
                            }

                            //return false;
                        }

                        LOGGER.Error("ParseTokensWithGrammar failed.");
                        errors += 1;
                    }
                } else {
                    // TODO: user should never see this.
                    LOGGER.Error("Internal compiler error.");
                    errors += 1;
                }
            }
            fclose(inFile);
        }

        result = (errors > 0);
#endif        

        // TODO: right now it is very jank where the previous pass writes to a file on disk.
        // then we take that file on disk and run our assembler on it.
        //
        // ideally, we want to have an in-memory representation of the PASM.
        if (result == 0)
        {
            // TODO: windows version is now divergent from the macOS version. still outputs to the bin directory.
#if defined(PLATFORM_WINDOWS)
            int &r = result;
            // TODO: why is there a conversion between const char * or whatever.
            r &= passembler((char *)outFilePath, "macOS"); // TODO(Noah): target independent, remove macOS.
            r &= CallSystem("mkdir bin");
            r &= pasm_x86_64(pasm_lines, "bin\\out.x86_64", MAC_OS); // TODO(Noah): target independent, remove macOS.
            DeallocPasm();
            r &= CallSystem("nasm -g -o bin\\out.obj -f win64 bin\\out.x86_64");
            r &= CallSystem("nasm -g -o bin\\exit.obj -f win64 " PSTDLIB_WINDOWS_DIR "\\Windows\\exit.s");
            r &= CallSystem("nasm -g -o bin\\stub.obj -f win64 " PSTDLIB_WINDOWS_DIR "\\Windows\\stub.s");
            r &= CallSystem("nasm -g -o bin\\print.obj -f win64 " PSTDLIB_WINDOWS_DIR "\\Windows\\console\\print.s");
            // TODO(Noah): Remove dependency on Visual Studio linker.
            r &= CallSystem("link /LARGEADDRESSAWARE /subsystem:console /entry:start bin\\out.obj bin\\exit.obj bin\\stub.obj bin\\print.obj \
                /OUT:bin\\out.exe Kernel32.lib");
#elif defined(PLATFORM_MAC)
            errors += 0 != passembler((char *)outFilePath, "macOS"
                // TODO: yuk, why are we using string to declare to the passembler that it's macos?
                // IIRC, this was because we wanted both the program interface and also the cli interface.
                // but we could fix this by having an enum that we just translate internally to the same path as the string.
                // or have a flag that passembler() takes in to indicate that we are calling internally vs. cmdline.
            );
            errors += 0 != pasm_x86_64(pasm_lines, "myProgram.x86_64", MAC_OS);
            DeallocPasm();
            errors += 0 != CallSystem("nasm -o myProgram.o -f macho64 myProgram.x86_64");
            errors += 0 != CallSystem("nasm -o " PSTDLIB_UNIX_DIR "/macOS/exit.o -f macho64 " PSTDLIB_UNIX_DIR "/macOS/exit.s");
            errors += 0 != CallSystem("nasm -o " PSTDLIB_UNIX_DIR "/macOS/console/print.o -f macho64 " PSTDLIB_UNIX_DIR "/macOS/console/print.s");
            errors += 0 != CallSystem("nasm -g -o " PSTDLIB_UNIX_DIR "/macOS/stub.o -f macho64 " PSTDLIB_UNIX_DIR "/macOS/stub.s");
            errors += 0 != CallSystem("ld -o myProgram -static myProgram.o " PSTDLIB_UNIX_DIR "/macOS/exit.o " PSTDLIB_UNIX_DIR "/macOS/console/print.o " PSTDLIB_UNIX_DIR "/macOS/stub.o");
#endif
        }

        PrintIfError(errors);

        return (errors > 1);

    }
    else if (0 == strcmp(l, "a") || 0 == strcmp(l, "about")) 
    {
        PrintAbout();
        return 0;

    } else if (0 == strcmp(l, "h") || 0 == strcmp(l, "help")) {
        
        PrintHelp();
        return 0;
    }

    printf("Unrecognized command '%s'. Enter 'help' to get a list of commands.\n", l);
    return 1;
}

void ptest_Codegen(char *inFilePath, int& errors)
{
    FILE *inFile = fopen(inFilePath, "r");
    // TODO: we could use the test name here; i.e. the one that's up at the lambda level.
    LOGGER.Log("Testing codegen for: %s", inFilePath);
    if (inFile == NULL) {
        LOGGER.Error("inFile of '%s' does not exist", inFilePath);
        errors += 1;
    } else {
        TokenContainer tokensContainer;
        RawFileReader tokenBirthplace;
        ppl_error_context bestErr = {};
        if (Lex(inFile, tokensContainer, &tokenBirthplace, &bestErr)) {
            if (VERBOSE) {
                tokensContainer.Print();
            }

            // Now we try to parse for the grammer object.
            // we know which specific grammer definition via the name of
            // the inFile that was given.

            const char *outFilePath = "program.out";

            const char *grammarDefName = "program";
            
            struct tree_node tree = {};
            
            bool r = ParseTokensWithGrammar(
                tokensContainer, 
                GRAMMAR.GetDef(grammarDefName),
                &tree, bestErr);
            
            //bool r = false;

            if (r) {
                
                if (VERBOSE) PrintTree(tree, 0);

                CG_Create(); // init the codegen system.
                
                RunCodegen(tree, outFilePath);
                
                CG_Release(); // shutdown the codegen system.
                
                DeallocTree(tree);
            }
            else {
                // emit the best error that we got back.
                {
                    uint32_t c = bestErr.c;
                    uint32_t line = bestErr.line;

                    // TODO: The below is likely to change when #import actually works.
                    // maybe it comes from the error because the source code that errors
                    // is in diff file.
                    const char *file = LOGGER.logContext.currFile;

                    const char *code = 
                        bestErr.codeContext ? bestErr.codeContext : "<unknown>"; // TODO.

                    LOGGER.EmitUserError(
                        file, line, c, code,
                        bestErr.errMsg ? bestErr.errMsg : "<unknown>" 
                    );

                    if (bestErr.kind == PPL_ERROR_KIND_PARSER)
                    {
                        LOGGER.Min("The almost-parsed AST:\n");
                        LOGGER.Min("%s\n",bestErr.almostParsedTree);
                    }

                    //return false;
                }

                LOGGER.Error("ParseTokensWithGrammar failed.");
                errors += 1;
            } 

        } else {
            LOGGER.Error("Lex failed.");
            errors += 1;
        }
    }
    fclose(inFile);
}

void ptest_Grammar(char *inFilePath, int&errors) {
    FILE *inFile = fopen(inFilePath, "r");
    LOGGER.Log("Testing grammar for: %s", inFilePath);
    if (inFile == NULL) {
        LOGGER.Error("inFile of '%s' does not exist", inFilePath);
        errors += 1;
    } else {
        TokenContainer tokensContainer;
        RawFileReader tokenBirthplace;
        ppl_error_context bestErr = {};
        if (Lex(inFile, tokensContainer, &tokenBirthplace, &bestErr)) {
            if (VERBOSE) {
                tokensContainer.Print();
            }

            // Now we try to parse for the grammar object.
            // we know which specific grammar definition via the name of
            // the inFile that was given.

            char grammarDefName[256] = {};
            
            // NOTE(Noah): Alright, so we got some truly dumbo code here :)
            
            #if defined(_MSC_VER)
            char slashCharacter = '\\';
            #else
            char slashCharacter = '/';
            #endif
            
            char *onePastLastSlash; 
            for (char *pStr = inFilePath; *pStr != 0; pStr++ ) {
                if (*pStr == slashCharacter) {
                    onePastLastSlash = pStr;
                }
            }
            onePastLastSlash++; // get it to one past the last slash.
            
            memcpy( grammarDefName, onePastLastSlash, strlen(onePastLastSlash) - 3 );
            //LOGGER.Log("grammarDefName: %s", grammarDefName);

            struct tree_node tree;// = {};

            bool r = ParseTokensWithGrammar(
                tokensContainer, 
                GRAMMAR.GetDef(grammarDefName),
                &tree, bestErr);
            
            //bool r = false;

            if (r) {
                PrintTree(tree, 0);
                DeallocTree(tree);
            }
            else {

                // emit the best error that we got back.
                {
                    uint32_t c = bestErr.c;
                    uint32_t line = bestErr.line;

                    // TODO: The below is likely to change when #import actually works.
                    // maybe it comes from the error because the source code that errors
                    // is in diff file.
                    const char *file = LOGGER.logContext.currFile;

                    const char *code = 
                        bestErr.codeContext ? bestErr.codeContext : "<unknown>"; // TODO.

                    LOGGER.EmitUserError(
                        file, line, c, code,
                        bestErr.errMsg ? bestErr.errMsg : "<unknown>" 
                    );

                    if (bestErr.kind == PPL_ERROR_KIND_PARSER)
                    {
                        LOGGER.Min("The almost-parsed AST:\n");
                        LOGGER.Min("%s\n",bestErr.almostParsedTree);
                    }

                    //return false;
                }

                LOGGER.Error("ParseTokensWithGrammar failed.");
                errors += 1;
            } 

        } else {
            LOGGER.Error("Lex failed.");
            errors += 1;
        }

        fclose(inFile);
    }
}


#define GENERATE_GROUND_TRUTH ptest_Lexer_gt_compound_ops
#include "preparse/compound_ops.gt.c"
#undef GENERATE_GROUND_TRUTH

int ptest_Lexer_all()
{
    Timer timer = Timer("lexer_all");
    LOGGER.InitFileLogging("w");

    // Initialize variables
    int errors = 0;

#define LEXER_TEST_NAME "compound_ops"

    {
        TokenContainer tokensContainer;
        // TODO: is discard qualifier here safe ?
        LOGGER.logContext.currFile = (char *)ModifyPathForPlatform("tests/preparse/" LEXER_TEST_NAME ".c").c_str();
        ptest_Lexer_gt_compound_ops( tokensContainer );
        ptest_Lexer(LOGGER.logContext.currFile, tokensContainer, errors);
    }

    PrintIfError(errors);
    timer.TimerEnd();
    return (errors > 0);
}

void ptest_Lexer(char *inFilePath, TokenContainer groundTruth, int &errors) {
    FILE *inFile = fopen(inFilePath, "r");
    LOGGER.Log("Testing lexer for: %s", inFilePath);
    if (inFile == NULL) {
        LOGGER.Error("inFile of '%s' does not exist", inFilePath);
        errors += 1;
    } else {
        TokenContainer tokensContainer;
        RawFileReader tokenBirthplace;
        ppl_error_context bestErr = {};
        if (Lex(inFile, tokensContainer, &tokenBirthplace, &bestErr)) {
            if (VERBOSE) {
                tokensContainer.Print();
            }

            // compare our container wih the ground truth container.
            {
                if (groundTruth.tokenCount != tokensContainer.tokenCount) {
                    LOGGER.Error("parsed %d tokens but expected %d", tokensContainer.tokenCount, groundTruth.tokenCount);
                    errors += 1;
                } else {
                    for (int i = 0; i < groundTruth.tokenCount; i++)
                    {
                        auto t1 = groundTruth.QueryDistance(i);
                        auto t2 = tokensContainer.QueryDistance(i);
                        
                        if (t1.type != t2.type) {
                            LOGGER.Error("token %d type mismatch", i );
                            errors += 1;
                        } else {
                            bool bMismatch = false;
                            switch(t1.type) {
                                case TOKEN_QUOTE: 
                                CASE_TOKEN_OP_COMPOUND
                                case TOKEN_KEYWORD: // TODO: really ought to have keywords not be generic and use TOKEN_OP_KEYWORD* for the different kinds.
                                case TOKEN_SYMBOL:
                                {
                                    bool bSame = strcmp( t1.str, t2.str ) == 0 && t1.beginCol == t2.beginCol && t1.line == t2.line;
                                    if (!bSame) bMismatch = true;
                                } break;
                                case TOKEN_UNDEFINED:
                                case TOKEN_TRUE_LITERAL:
                                case TOKEN_FALSE_LITERAL:
                                case TOKEN_NULL_LITERAL:
                                case TOKEN_UINT_LITERAL:
                                case TOKEN_INTEGER_LITERAL:
                                case TOKEN_DOUBLE_LITERAL:
                                case TOKEN_FLOAT_LITERAL:
                                case TOKEN_ENDL:
                                case TOKEN_CHARACTER_LITERAL: // NOTE: this is merely the unicode point.
                                CASE_TOKEN_OP
                                case TOKEN_PART:
                                default:
                                if (memcmp( &t1, &t2, sizeof(struct token) ) != 0) {
                                        bMismatch = true;
                                    } break;                    
                            }
                            if (bMismatch) {
                                LOGGER.Error("token %d bytes mismatch", i );
                                errors += 1;
                            }
                        }                        
                    }
                }
            }

        } else {
            LOGGER.Error("Lex() failed.");
            errors += 1;
        }
        fclose(inFile);
    }
}

void ptest_Preparser(char *inFilePath, int &errors) {
    FILE *inFile = fopen(inFilePath, "r");
    LOGGER.Log("Testing parser for: %s", inFilePath);
    if (inFile == NULL) {
        LOGGER.Error("inFile of '%s' does not exist", inFilePath);
        errors += 1;
    } else {
        TokenContainer tokensContainer;
        RawFileReader tokenBirthplace;
        ppl_error_context bestErr = {};
        if (Lex(inFile, tokensContainer, &tokenBirthplace, &bestErr)) {
            if (VERBOSE) {
                tokensContainer.Print();
            }
            // Now add on the preparsing.
            struct tree_node tn;
            Preparse(tokensContainer, tn);
        } else {
            LOGGER.Error("Lex() failed.");
            errors += 1;
        }
    }
    fclose(inFile);
}

// TODO: this stuff works on Windows? some of the paths have `\\` so I suspect so.
void ptest_wax64(char *inFilePath, int &errors) {
    LOGGER.Log("Testing assembler for: %s", inFilePath);
    int r = passembler(inFilePath, "macOS"); // TODO(Noah): target independent, remove macOS.
    r = pasm_x86_64(pasm_lines, "bin/out.x86_64", MAC_OS); // TODO(Noah): target independent, remove macOS.
    DeallocPasm();
    r = CallSystem("nasm -g -o bin\\out.obj -f win64 bin/out.x86_64");
    r = CallSystem("nasm -g -o bin\\exit.obj -f win64 " PSTDLIB_UNIX_DIR "/Windows/exit.s");
    r = CallSystem("nasm -g -o bin\\stub.obj -f win64 " PSTDLIB_UNIX_DIR "/Windows/stub.s");
    r = CallSystem("nasm -g -o bin\\print.obj -f win64 " PSTDLIB_UNIX_DIR "/Windows/console/print.s");
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

void ptest_ax64(char *inFilePath, int &errors) {
    LOGGER.Log("Testing assembler for: %s", inFilePath);
    int r = passembler(inFilePath, "macOS");
    r = pasm_x86_64(pasm_lines, "bin/out.x86_64", MAC_OS);
    DeallocPasm();
    r = CallSystem("nasm -o bin/out.o -f macho64 bin/out.x86_64");
    r = CallSystem("nasm -o bin/exit.o -f macho64 " PSTDLIB_UNIX_DIR "/macOS/exit.s");
    r = CallSystem("nasm -o bin/print.o -f macho64 " PSTDLIB_UNIX_DIR "/macOS/console/print.s");
    r = CallSystem("nasm -g -o bin/stub.o -f macho64 " PSTDLIB_UNIX_DIR "/macOS/stub.s");
    r = CallSystem("ld -o bin/out -static bin/out.o bin/exit.o bin/print.o bin/stub.o");
    r = CallSystem("bin/out");
    errors = (r != 0 );
    if (errors > 0) {
        LOGGER.Error("Completed with %d error(s)", errors);
        LOGGER.Error("Return code: %d", r);
    } else {
        LOGGER.Success("Completed with 0 errors.");
    }
}

// TODO(Noah): We can even modularize the two different version of the function that we use
// to modularize the code!

int RunPtestFromInFile(void (*ptest)(char *inFilePath, int &errors), const char *testName, const char *cwd) {
    printf("NOTE: cwd is set to %s\n", cwd);
    char *inFile = GetInFile();
    char *inFilePath = SillyStringFmt("%s%s", cwd, inFile);
    Timer timer = Timer(testName);
    LOGGER.InitFileLogging("w");
    LOGGER.logContext.currFile = inFilePath;
    int errors = 0;
    ptest(inFilePath, errors);
    PrintIfError(errors);
    timer.TimerEnd();
    return (errors > 0);
}

int RunPtestFromInFile(void (*ptest)(char *inFilePath, int &errors), const char *testName, const char *cwd, const char *inFile) {
    printf("NOTE: cwd is set to %s\n", cwd);
    char *inFilePath = SillyStringFmt("%s%s", cwd, inFile);
    Timer timer = Timer(testName);
    LOGGER.InitFileLogging("w");
    LOGGER.logContext.currFile = inFilePath;
    int errors = 0;
    ptest(inFilePath, errors);
    PrintIfError(errors);
    timer.TimerEnd();
    return (errors > 0);
}

#if defined(_MSC_VER)
#include <tchar.h>
#endif

#include <iostream>
#include <string>

#define NC_PAL_HPP_IMPL
#include <nc/pal.hpp>

int RunPtestFromCwd(
    void (*ptest)(char *inFilePath, int &errors), 
    bool (*validate)(const char *fileName),
    const char *testName, 
    const char *cwd
) {

    Timer timer = Timer(testName);
    LOGGER.InitFileLogging("w");

    // Initialize variables
    int errors = 0;

    using namespace nc;

    const char *dirName = cwd;
    std::string searchPath = std::string(cwd) + ModifyPathForPlatform("/*");
    pal::file_search_t fSearch;
    pal::file_search_find_data_t findData;
    if ( pal::createFileSearch( searchPath.c_str(), &fSearch, &findData ) )
    {
        do {
            if (validate(findData.name)) {          
            //TODO: this is Sa hack. not sure why the code was written this way to cause the
            // double path issue that we were getting.
#if defined(PLATFORM_MAC)
                char *fileName = SillyStringFmt("%s", findData.name);// findData.name;
#else
                char *fileName = SillyStringFmt(ModifyPathForPlatform("%s/%s").c_str(), dirName, findData.name);
#endif
                LOGGER.logContext.currFile = fileName;
                ptest(fileName, errors);
            }
        } while (pal::fileSearchGetNext(&fSearch, &findData));

        pal::fileSearchFree( &fSearch );
    }

    PrintIfError(errors);
    timer.TimerEnd();
    return (errors > 0);
}

// splits the command line by in-place modifying the string (inserts null terminators).
int splitCommandLine(char * commandBuffer, char* args[], int maxargs, char delimiter){

    int nargs = 0;
    char *sPtr = commandBuffer;
    char *arg = commandBuffer;

    if (sPtr == NULL) goto splitCommandLine_end;

    // skip whitespace.
    while( (*arg == delimiter) ) arg++;
    sPtr = arg;

    while ( *arg != 0 ) {

        if (nargs < maxargs) args[nargs] = arg;
        // if (nargs) LOG("arg: %s\n", args[nargs - 1]);
        nargs++;
     
        while( (*sPtr != delimiter) && (*sPtr != 0) ) sPtr++; // get to whitespace.
        while( (*sPtr == delimiter) ) *sPtr++ = 0; // skip whitespace and set to null terminator at the same time.

        arg = sPtr;
    }

    //LOG("arg: %s\n", args[nargs - 1]);

    splitCommandLine_end:

    return nargs;
}