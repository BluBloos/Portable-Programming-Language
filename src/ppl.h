// Common include for all files.

/* PROJECT DEPENDENCIES */
#include <stdio.h> 
#include <string>
#include <vector>
// #include <time.h>
#include <stdarg.h>
//#include <x86intrin.h>
/* PROJECT DEPENDENCIES */

typedef unsigned long long uint64;
typedef unsigned int uint32;
#define INTERNAL static
#define PERSISTENT static

enum target_platform {
    MAC_OS,
    WINDOWS,
    POSIX, // TODO(Noah): Should this be UNIX? What is going on here?
    WEB,
    WEB_SCRIPT
};

/* PROGRAM GLOBALS */
#include <logger.h>
#include <mem.h>
Logger LOGGER;
ConstMemoryArena MEMORY_ARENA(1024 * 1024 * 60); // 60 MB.
// Compiler parameters.
enum target_platform PLATFORM = POSIX;
bool VERBOSE = false;
/* PROGRAM GLOBALS */

/* OTHER COMPILATION UNITS */
#include <lexer.h>
#include <timing.h>
/* OTHER COMPILATION UNITS */

