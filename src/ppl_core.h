// Common include for all files.

#if defined(_WIN32)
    #define PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define PLATFORM_MAC
#else
    #define PLATFORM_UNIX
#endif

/* PROJECT DEPENDENCIES */
#include <stdio.h> 
#include <string>
#include <vector>
// #include <time.h>
#include <stdarg.h>
//#include <x86intrin.h>
#include <std/utf8.c>
#include <unordered_map>
#include <dirent.h>
#ifdef PLATFORM_WINDOWS
    #include <Windows.h>
#endif
/* PROJECT DEPENDENCIES */

typedef unsigned long long uint64;
typedef unsigned int uint32;
#define INTERNAL static
#define PERSISTENT static
#define Assert(b) (b) ? (void)0 : ( LOGGER.Error("Assertion failure!"), abort()) 

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
bool VERBOSE = true;
/* PROGRAM GLOBALS */

/* SILLY THINGS */
char __silly_buff[256];
char *SillyStringFmt(char *fmt, ...) {
    va_list args;
    va_start (args, fmt);
    vsprintf(__silly_buff, fmt, args);
    va_end (args);
    return __silly_buff;
}

// Checking if a starts with b, returns with appropriate boolean value.
bool SillyStringStartsWith(const char* a, const char* b) {
    char *pStrA = (char *)a;
    char *pStrB = (char *)b;
    for ( ; *pStrA != 0 && *pStrB != 0 ; (pStrA++, pStrB++) ) {
        if (*pStrA != *pStrB)
            break;
    }
    return (*pStrB == 0);
}
/* SILLY THINGS */

/* OTHER COMPILATION UNITS */
#include <lexer.h>
#ifdef PLATFORM_WINDOWS
    #include <win32_timing.h>
#else
    #include <timing.h>
#endif
#include <grammer.h>
#include <tree.h>
#include <syntax.h>
/* OTHER COMPILATION UNITS */



