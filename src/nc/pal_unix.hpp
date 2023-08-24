
#if !defined( NC_PAL_UNIX_HPP )
#define NC_PAL_UNIX_HPP

#if defined( __GNUC__ )

// check if on win32
#if !defined( _WIN32 )
#include <glob.h>
#endif

namespace nc {
    namespace pal {

#if !defined( _WIN32 )

        // if using mingw, these ideas are not supported.

        // TODO: none of the code below is tested. but I bet it works.

        static_assert(  sizeof(file_search_t) >= (sizeof(glob_t) + sizeof(size_t)),
            "private data of file_search_t must be large enough." );

        bool createFileSearch(const char *search, file_search_t *fSearch, file_search_find_data_t *findData)
        {
            assert(fSearch);

            glob_t *gb = (glob_t *)fSearch;
            size_t &lastIdx = *(size_t *)(gb+1);
            lastIdx = 0;

            // TODO: do we need an error function?
            int (*errfunc)(const char *epath, int eerrno) = NULL;
            if (glob(search, 0, errFunc, gb) == 0)
            {
                return fileSearchGetNext(fSearch, findData);
            }
            return false;

        }

        bool fileSearchGetNext( file_search_t *fSearch, file_search_find_data_t *findData)
        {
            glob_t *gb = (glob_t *)fSearch;
            size_t &lastIdx = *(size_t *)(gb+1);

            if (++lastIdx < gb->gl_pathc) {
                findData->name = gb->gl_pathv[lastIdx];
                return true;
            }

            return false;
        }

        void fileSearchFree( file_search_t *fSearch )
        {
            glob_t *gb = (glob_t *)fSearch;
            globfree(gb);            
        }
#endif

    }
}

#endif // if on GNU.

#endif // include header.