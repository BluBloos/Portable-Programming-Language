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