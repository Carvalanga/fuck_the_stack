#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

//-------------------CONFIG-------------------
    #define STACK_DEBUG
    #define STACK_PROTECTION
//--------------------------------------------

//-------------------DETAILS------------------
    #ifdef STACK_DEBUG
        #define ON_DEBUG(...) __VA_ARGS__
    #else
        #define ON_DEBUG(...)
    #endif

    #ifdef STACK_PROTECTION

        #include "hash_func.h"

        #define CANARY_HEX_VAL 0xEDA
        #define POISON 0xBADF00D
        #define ON_PROTECTION(...) __VA_ARGS__
        #define DATA_DEBUG_SIZE size_t data_debug_size_byte

        typedef uint64_t canary_t;

    #else
        #define CANARY(number)
        #define ON_PROTECTION(...)
    #endif
//-----------------------------------------------
#endif
