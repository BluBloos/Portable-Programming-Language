#ifndef PPL_CORE_H
#define PPL_CORE_H
// Common include for all files.

#if defined(_WIN32)
#define PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM_MAC
#else
#define PLATFORM_UNIX
#endif

#include <nc_defer.h>

/* PROJECT DEPENDENCIES */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
// #include <time.h>
#include <stdarg.h>

#if !defined(_MSC_VER)
#include <unistd.h>
#include <dirent.h>
#else

// workaround because we do not have unistd.h
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;

#endif

// #include <x86intrin.h>

#define STB_DS_IMPLEMENTATION
#define STBDS_NO_SHORT_NAMES
#include <stb_ds.h>
// TODO(Noah): Remove dependency on unordered_map.
#include <unordered_map>
#include <math.h>
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#include <WinCon.h>
#endif
/* PROJECT DEPENDENCIES */

typedef unsigned long long uint64;
typedef unsigned int uint32;
#define INTERNAL static
#define PERSISTENT static
#define Assert(b) (b) ? (void)0 : (LOGGER.Error("Assertion failure!"), abort())
#define SafeSubtract(Value, Subtractor) ((Value >= Subtractor) ? Value - Subtractor: 0)
#define ColorError "\033[0;33m"
#define ColorHighlight "\033[0;36m"
#define ColorNormal "\033[0m"

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

bool VERBOSE = true; // TODO: hook with build system (e.g. CMake).

/* PROGRAM GLOBALS */

/* SILLY THINGS */
char __silly_buff[256];

// Uses printf syntax to format a string, but it returns as a parameter the pointer to the null-terminated
// string. Be warned that every call to this function destroys the result of the last call to this function.
char *SillyStringFmt(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsprintf(__silly_buff, fmt, args);
    va_end(args);
    return __silly_buff;
}



// Returns true if the character c is inside the SillyString a.
bool SillyStringCharIn(const char *a, char c) {
    for (char *pStr = (char *)a; *pStr != 0; pStr++) {
        if (*pStr == c)
            return true;
    }
    return false;
}

// Returns the length of the silly string.
unsigned int SillyStringLength(const char *str) {
    unsigned int r = 0;
    while (*str++ != 0) { r++; }
    return r;
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

bool SillyStringEquals(const char *a, const char *b) {
    bool r = SillyStringLength(a) == SillyStringLength(b);
    if (!r) return false;
    return SillyStringStartsWith(a, b); 
}

// Parses the silly string as an unsigned integer, and returns the interpreted value.
// if the string does not represent an unsigned integer, the behaviour of this function is
// undefined.
unsigned int SillyStringToUINT(char *str)
{
	unsigned int result = 0;
	unsigned int strLength = SillyStringLength(str);
	unsigned int placeValue = (int)powf(10.0f, (strLength - 1.0f) );
    for (unsigned int x = 0; x < strLength; x++)
	{
		result += (SafeSubtract(*str, '0')) * placeValue;
		str++;
		placeValue = placeValue / 10;
	}
	return result;
}

// TODO: there is pretty much a duplicate function in lexer.h
//
// Return true if the string can be represented as a number,
// returns false otherwise. decimalFlag is set to true if the 
// represented number is a decimal as opposed to an integer.
bool SillyStringIsNumber(char *str, bool &decimalFlag) {
    if (*str == 0) 
        return false;
    unsigned int strLength = SillyStringLength(str);
    if (str[0] == '.' || str[strLength - 1] == '.')
        return false;
    for (unsigned int i = 0; i < strLength; i++) {
        char c = str[i];
        switch(c) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            break;
            case '.':
            decimalFlag = true;
            break;
            default:
            return false;
        }
    } 
    return true;
}

// TODO(Noah): Sometime this function throws an error when we are in the debugger. And it seems to happen 
// after some amount of time passes.
void SillyStringRemove0xA(char* l) {
    for (char *pStr = l; *pStr != 0; pStr++) {
        if (*pStr == '\n') 
            *pStr = 0;
    }
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
// end of Stretchy buffer things

/* SILLY THINGS */

#ifdef PLATFORM_WINDOWS
    // TODO(Noah): Make faster and less "dumb". Make compliant with the behaviour of getline so that in the
    // future, when someone who is not me tries to call getline, it works as expected.
    ssize_t getline(char **l, size_t *n, FILE *streamIn) {

        /*
            https://man7.org/linux/man-pages/man3/getline.3.html
            On success, return the number of characters read, including the delimiter character, but not
            including the terminating null byte ('\0'). This value can be
            used to handle embedded null bytes in the line read.

            return -1 on failure to read a line (including end-of-file condition).
            In the event of a failure, errno is set to indicate the error.
        */

        std::string str = "";

        char c = fgetc(streamIn);    
        while (c != EOF) {
            str += c;
            if (c == '\n') {
                break;
            }    
            c = fgetc(streamIn);
        }

        if (*l == NULL) {
            // Allocate a buffer to store the line.
            unsigned int memSize = (str.size() + 1) * sizeof(char);
            *l = (char *)malloc(memSize);
            memcpy(*l, str.c_str(), memSize); // this will include the null-terminator.
        }

        if (c == EOF) {
            return -1;
        } else {
            return (ssize_t)str.size();
        }
    }
#endif

class PFileWriter {
public:
    FILE *handle;
    unsigned int indentation;
    bool freshNewline;
    PFileWriter(FILE *handle) : handle(handle), indentation(0), freshNewline(true) {}
    PFileWriter(char *file) : indentation(0), freshNewline(true) {
        handle = fopen(file, "w");
    }
    ~PFileWriter() { fclose(handle); }
    void IncreaseIndenation(unsigned int amount) { indentation += amount; }
    void DecreaseIndentation(unsigned int amount) { indentation -= amount; }
    // TODO(Noah): Make this take in a char *fmt string and ... variadic arguments.
    void write(char *str) {
        size_t n = 0 ;
        std::string currentWrite = "";        
        while (n < strlen(str)) {
            char c = str[n];
            if (c == '\n') {
                fprintf(handle, "%s", currentWrite.c_str());
                currentWrite = "";
                fprintf(handle, "\n");
                freshNewline = true;
            } else {
                if (freshNewline) {
                    std::string sillyWhitespace = ""; 
                    unsigned int i = 0;
                    while(i++ < indentation) {sillyWhitespace += ' ';}
                    fprintf(handle, "%s", sillyWhitespace.c_str());
                }
                currentWrite += c;
                freshNewline = false;
            }
            n += 1;
        }
        if (currentWrite != "") {
            fprintf(handle, "%s", currentWrite.c_str());
        }
    }   
};

struct ppl_str_view
{
    char *str;
    uint32_t len;
};

// TODO: Make this file (ppl_core.h) a *.hpp

/* OTHER COMPILATION UNITS */
#include <ppl_error.hpp>
#include <lexer.h>
#ifdef PLATFORM_WINDOWS
#include <win32_timing.h>
#else
#include <timing.h>
#endif
#include <grammar.h>
#include <syntax.h>
#include <tree.h>
// backend things :P
#include <assembler.h>
#include <x86_64.h>
/* OTHER COMPILATION UNITS */

#endif
