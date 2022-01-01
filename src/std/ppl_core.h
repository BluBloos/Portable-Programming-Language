#ifndef PPL_CORE
#define PPL_CORE
// TODO(Noah): Implement the len operator on string and Array data types.
#define len(arr) arr.arrSize
typedef unsigned long long uint64;
typedef unsigned int uint32; 
#define string char *
namespace PPL {
    template <class T>
    class Array {
        public:
        T *items;
        unsigned int itemCount; /* in items */
        unsigned int arrSize; /* in items */
        T &operator[](unsigned int i);
    };
}
#endif