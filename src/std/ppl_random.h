#ifndef PPL_RANDOM
#define PPL_RANDOM
#include <stdlib.h>
namespace ppl {
    void _seed_rand(unsigned int seed) {
        srand(seed);
    }
    int _rand() {
        return rand();
    };
}

#endif