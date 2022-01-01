#include <ppl_core.h>

template <class T> T& PPL::Array<T>::operator[](unsigned int i) {
    // TODO(Noah): It is likely we want to change this...
    return this->items[i];
}