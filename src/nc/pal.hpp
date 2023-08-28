
// TODO: factor out this file and combine with https://github.com/BluBloos/Automata-Engine.

#if !defined(NC_PAL_HPP)
#define NC_PAL_HPP

// Unix Version.
namespace nc {
    namespace pal {

        // on the Windows side stores a HANDLE.
        // on the Unix side this contains stores a glob_t.
        struct file_search_t
        {
            uint32_t privData[128];
        };

        // TODO: need anything more here?
        struct file_search_find_data_t
        {
            const char *name;
        };

        bool createFileSearch(const char *search, file_search_t *fSearch, file_search_find_data_t *findData);

        // returns true if we got something. so will return false if there is nothing left
        // after what we last got from this function. 
        bool fileSearchGetNext( file_search_t *fSearch, file_search_find_data_t *findData);

        void fileSearchFree( file_search_t *fSearch );

    };
};

#if defined( NC_PAL_HPP_IMPL )

#if defined( __GNUC__ )
#include "pal_unix.hpp"
#endif

#if defined( _MSC_VER )
#include "pal_windows.hpp"
#endif

#endif // if impl.

#endif // include guard.