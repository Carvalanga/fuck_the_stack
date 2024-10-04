#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <stdint.h>

#include "config.h"

#if defined STACK_DEBUG || defined STACK_PROTECTION
    #define STACK_ASSERT(stack)                             \
        do {                                                \
            if(stack_verify(stack) != STACK_NO_ERROR){      \
                log_close();                                \
                assert(0);                                  \
            }                                               \
        } while(0)
#else
    #define STACK_ASSERT(stack)
#endif

const size_t MINIMAL_STACK_SIZE = 16;
const double MEGABYTE = 1048578;
const double MULT_LIMIT_SIZE = MEGABYTE * 8;

typedef int stack_element_t;

enum STACK_ERROR {
    STACK_NO_ERROR = 0,
    STACK_IS_NULL,
    STACK_DATA_IS_NULL,
    STACK_OVERFLOW,
    STACK_UNDERFLOW,
    STACK_BROKEN
};

struct STACK {
    ON_PROTECTION(
    uint64_t canary1);

    ON_DEBUG(
    const char* name;
    const char* born_file_name;
    int born_line);

    ON_PROTECTION(
    size_t data_debug_size_byte);

    ON_PROTECTION(
    int hash_main;
    int hash_data);

    stack_element_t* data;
    size_t data_size;
    size_t item_number;
    STACK_ERROR error;

    ON_PROTECTION(
    uint64_t canary2);
};

STACK_ERROR stack_ctor(STACK* stack, size_t size ON_DEBUG(, const char* stack_name, const char* file_name, int line));

STACK_ERROR stack_dtor(STACK* stack);

void stack_dump(STACK* stack);

STACK_ERROR stack_push(STACK* stack, stack_element_t element);

STACK_ERROR stack_pop(STACK* stack, stack_element_t* pop_to_element);

STACK_ERROR stack_verify(STACK* stack);

#endif //STACK_H
