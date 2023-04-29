#if !defined( NC_PAL_WINDOWS_HPP )
#define NC_PAL_WINDOWS_HPP

#if defined( _MSC_VER )

#include <Windows.h>
#include <fileapi.h>
#include <minwinbase.h>
#include <stdio.h>

namespace nc {
    namespace pal {

        static void LogLastError(DWORD lastError, const char *message) {
            char *lpMsgBuf;
            FormatMessage(
                // FORMAT_MESSAGE_FROM_SYSTEM     -> search the system message-table resource(s) for the requested message
                // FORMAT_MESSAGE_ALLOCATE_BUFFER -> allocates buffer, places pointer at the address specified by lpBuffer
                // FORMAT_MESSAGE_IGNORE_INSERTS  -> Insert sequences in the message definition such as %1 are to be
                //                                   ignored and passed through to the output buffer unchanged.
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                lastError,  // dwMessageId
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &lpMsgBuf, // lpBuffer
                0, // no need for nSize with FORMAT_MESSAGE_ALLOCATE_BUFFER
                NULL // not using any insert values such as %1
            );

            // TODO(Noah): Remove newline character at the end of lpMsgBuf
            printf("%s with error=%s", message, lpMsgBuf);

            LocalFree(lpMsgBuf);
        }

        static_assert(  sizeof(file_search_t) >= (sizeof(WIN32_FIND_DATAA) + sizeof(HANDLE) ),
            "private data of file_search_t must be large enough." );
        
        bool createFileSearch(const char *search, file_search_t *fSearch, file_search_find_data_t *findData)
        {
            HANDLE &dir = *(HANDLE *)fSearch;
            WIN32_FIND_DATAA &win32FindData = *(WIN32_FIND_DATAA *)(&dir+1);

            dir = FindFirstFileA(search, &win32FindData);
            if (dir == INVALID_HANDLE_VALUE) {

                DWORD error = GetLastError();
                LogLastError(error, "Could not read file");

                return false;
            }
            else {
                // write out win32 data to findData.
                findData->name = win32FindData.cFileName;
                return true;
            }
        }

        bool fileSearchGetNext( file_search_t *fSearch, file_search_find_data_t *findData)
        {
            HANDLE &dir = *(HANDLE *)fSearch;
            WIN32_FIND_DATAA &win32FindData = *(WIN32_FIND_DATAA *)(&dir+1);

            BOOL result = false;

            // iterate to next file.            
            while ( result = FindNextFileA(dir, &win32FindData) )
            {
                if ( (result != 0) && win32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
                break;
            }

            if (result == 0)
            {
                // fail condition.
                return false;
            }
            else
            {
                // write out win32 data to findData.
                findData->name = win32FindData.cFileName;
                return true;
            }
        }

        void fileSearchFree( file_search_t *fSearch )
        {
            HANDLE dir = *(HANDLE *)fSearch;
            FindClose(dir);           
        }

    }
}


#endif // if using msvc.

#endif // include guard.
