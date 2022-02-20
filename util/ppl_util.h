#include <ppl_core.h>

void CheckErrors(int errors) {
    if (errors > 0)
        LOGGER.Error("Completed with %d error(s)", errors);
    else
        LOGGER.Success("Completed with 0 errors.");
}

#ifdef PLATFORM_WINDOWS
    // TODO(Noah): Make faster and less "dumb". Make compliant with the behaviour of getline so that in the
    // future, when someone who is not me tries to call getline, it works as expected.
    void getline(char **l, size_t *n, FILE *streamIn) {

        size_t nVal = 0;    
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
    }
#endif

#define ColorError "\033[0;33m"
#define ColorHighlight "\033[0;36m"
#define ColorNormal "\033[0m"

void RemoveEndline(char* l) {
    for (char *pStr = l; *pStr != 0; pStr++) {
        if (*pStr == '\n') 
            *pStr = 0;
    }
}