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
#include <stdlib.h>
#include <string>
#include <cstring>
#include <vector>
// #include <time.h>
#include <stdarg.h>
#include <unistd.h>
//#include <x86intrin.h>
#include <dirent.h>
#include <unordered_map>
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#endif
/* PROJECT DEPENDENCIES */

typedef unsigned long long uint64;
typedef unsigned int uint32;
#define INTERNAL static
#define PERSISTENT static
#define Assert(b) (b) ? (void)0 : (LOGGER.Error("Assertion failure!"), abort())

enum target_platform {
    MAC_OS,
    WINDOWS,
    // TODO(Noah): Be serious and investigate the difference between POSIX and UNIX.
    // We need to get these targets right.
    POSIX,
    WEB,
    WEB_SCRIPT
};

/* PROGRAM GLOBALS */
#include <logger.h>
#include <mem.h>
#include <utf8.h>
Logger LOGGER;
ConstMemoryArena MEMORY_ARENA(1024 * 1024 * 60); // 60 MB.
// Compiler parameters.
enum target_platform PLATFORM = POSIX;
bool VERBOSE = true;
/* PROGRAM GLOBALS */

/* SILLY THINGS */
char __silly_buff[256];

// Uses printf syntax to format a string, but it returns as a paramter the pointer to the null-terminated
// string. Be warned that every call to this function destroys the result of the last call to this function.
char *SillyStringFmt(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsprintf(__silly_buff, fmt, args);
    va_end(args);
    return __silly_buff;
}

// Returns true if a starts with b, false otherwise.
bool SillyStringStartsWith(const char *a, const char *b) {
    char *pStrA = (char *)a;
    char *pStrB = (char *)b;
    for (; *pStrA != 0 && *pStrB != 0; (pStrA++, pStrB++)) {
        if (*pStrA != *pStrB)
            break;
    }
    return (*pStrB == 0);
}

// Returns true if the character c is inside the SillyString a.
bool SillyStringCharIn(const char *a, char c) {
    for (char *pStr = (char *)a; *pStr != 0; pStr++) {
        if (*pStr == c)
            return true;
    }
    return false;
}

// NOTE(Noah): Stretchy buffers adapated from the cryptic C code of https://nothings.org/stb/stretchy_buffer.txt
// Stretchy buffers basically work like so: A block of memory is allocated to store the current count, total element size,
// plus all the elements. The array pointer that was passed in originally is modified in place with the new element pointer,
// which is offset by 2 in the allocated block (just after the count and total element size).
// 
// All stretchy buffers must begin as a null pointer.

// Inits the stretchy buffer.
#define StretchyBufferInit(a)             (StretchyBuffer_Grow(a,1))
// Frees the strechy buffer. Warning: the array a will be a dangling pointer after this call.
#define StretchyBufferFree(a)             ((a) ? free(StretchyBuffer_GetMetadataPtr(a)), 0 : 0)
// Pushes a new element to the stretchy buffer.
#define StretchyBufferPush(a,v)           (StretchyBuffer_MaybeGrow(a,1), (a)[StretchyBuffer_GetCount(a)++] = (v))
// Returns a reference to the count of the stretchy buffer.
#define StretchyBufferCount(a)            ((a) ? StretchyBuffer_GetCount(a) : 0)
// Returns a reference to the last element of the stretchy buffer.
#define StretchyBufferLast(a)             ((a)[StretchyBuffer_GetCount(a)-1])

// Returns and deletes the last element from inside the stretchy buffer.
#define StretchyBufferPop(a)              ((a)[StretchyBuffer_GetCount(a)--]) 

#define StretchyBuffer_GetMetadataPtr(a)  ((int *) (a) - 2)
#define StretchyBuffer_GetBufferSize(a)   StretchyBuffer_GetMetadataPtr(a)[0]
#define StretchyBuffer_GetCount(a)        StretchyBuffer_GetMetadataPtr(a)[1]

#define StretchyBuffer_NeedGrow(a,n)      ((a) == 0 || StretchyBuffer_GetCount(a) + n >= StretchyBuffer_GetBufferSize(a))
#define StretchyBuffer_MaybeGrow(a,n)     (StretchyBuffer_NeedGrow(a,(n)) ? StretchyBuffer_Grow(a,n) : (void)0)
#define StretchyBuffer_Grow(a,n)          StretchyBuffer_Growf((void **) &(a), (n), sizeof(*(a)))

static void StretchyBuffer_Growf(void **arr, int increment, int itemsize)
{
   int m = *arr ? 2 * StretchyBuffer_GetBufferSize(*arr) + increment : increment + 1;
   void *p = realloc(*arr ? StretchyBuffer_GetMetadataPtr(*arr) : 0, itemsize * m + sizeof(int) * 2);
   Assert(p);
   if (p) {
      if (!*arr) ((int *) p)[1] = 0;
      *arr = (void *) ((int *) p + 2);
      StretchyBuffer_GetBufferSize(*arr) = m;
   }
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
#include <syntax.h>
#include <tree.h>
/* OTHER COMPILATION UNITS */
