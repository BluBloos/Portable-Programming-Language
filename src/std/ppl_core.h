#ifndef PPL_CORE
#define PPL_CORE
// TODO(Noah): Implement the len operator on string and Array data types.
#define len(arr) arr.itemCount
typedef unsigned long long uint64;
typedef unsigned int uint32; 
#define string char *
#include <stdlib.h>
#include <stdio.h>
namespace PPL {
    template <class T>
    class _Array {
        public:
        _Array();
        ~_Array();
        T *items;
        unsigned int itemCount; /* in items */
        // Bounding amount for underlying allocated memory.
        unsigned int arrSize; /* in items */
        T &operator[](unsigned int i);
        void append(T item);
    };

    template <class T> _Array<T>::_Array() :
        items(NULL), itemCount(0), arrSize(0) {
    }

    template <class T> _Array<T>::~_Array() {
        free(this->items);
    }

    template <class T> T& _Array<T>::operator[](unsigned int i) {
        // TODO(Noah): It is likely we want to change this...
        // Make it so that if I is a bad index we return a "bad" value of T.
        return this->items[i];
    }

    template <class T> void _Array<T>::append(T item) {
        if (this->itemCount == this->arrSize) {
            // TODO(Noah): Maybe implement something more intelligent
            // for realloc.
            int bytes = (this->arrSize + 100) * sizeof(T);
            T* newPtr = (T *)realloc(this->items, bytes);
            // TODO(Noah): Do something other than silently fail 
            // in the case of realloc fail!
            if (newPtr != NULL)
                {this->items = newPtr;
                this->arrSize += 100; 
                this->items[this->itemCount++] = item;}
            else
                printf("\n[RUNTIME WARNING]:Failed to realloc for dynamic array.\n");
            

        } else {
            this->items[this->itemCount++] = item;
        }
    }
}
#endif