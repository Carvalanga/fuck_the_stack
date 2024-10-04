#include <stdlib.h>

#include "hash_func.h"

int calc_hash(char* ptr, size_t n_byte) {

    int hash = 5381;
    for (size_t i = 0; i < n_byte; i++) {
        hash = ((hash << 5) + hash) + ptr[i];
    }

    return hash & 0xFFFFFFFF;
}

int calc_hash_with_ignore(char* ptr, size_t n_byte, char* ptr_to_ignore, size_t n_byte_to_ignore) {

    int hash = 5381;
    for (size_t i = 0; i < n_byte; i++) {
        if ((ptr + i) == ptr_to_ignore) {
            while(n_byte_to_ignore--) {
                i++;
            }
        }
        hash = ((hash << 5) + hash) + ptr[i];
    }

    return hash & 0xFFFFFFFF;
}
