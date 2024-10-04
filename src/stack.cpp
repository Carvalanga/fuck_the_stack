#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <stdarg.h>
#include <assert.h>

#include "config.h"
#include "log.h"
#include "safe_wrappers.h"
#include "stack.h"

static STACK_ERROR stack_size_increase(STACK* stack);

static STACK_ERROR stack_size_decrease(STACK* stack);

static void stack_change_size(STACK* stack, size_t new_size);

static void stack_data_create(STACK* stack, size_t size);

ON_PROTECTION(static void stack_fill_poison(STACK* stack));
ON_PROTECTION(static void stack_recalc_hash(STACK* stack));

STACK_ERROR stack_ctor(STACK* stack, size_t size ON_DEBUG(, const char* name, const char* born_file_name, int born_line)) {


    size = size < MINIMAL_STACK_SIZE ? MINIMAL_STACK_SIZE : size;
    stack->data_size = size;

    stack_data_create(stack, size);

    ON_DEBUG(
    stack->name = name;
    stack->born_file_name = born_file_name;
    stack->born_line = born_line;
    );

    ON_PROTECTION(stack_fill_poison(stack));
    ON_PROTECTION(stack_recalc_hash(stack));

    STACK_ASSERT(stack);

    return STACK_NO_ERROR;

    else {
        log_write("!!!! ERROR: STACK IS NULL --- %s !!!!", __func__);
        fprintf(stderr, "!!!! ERROR: STACK IS NULL --- %s !!!!", __func__);
        return STACK_IS_NULL;
    }
}

STACK_ERROR stack_dtor(STACK* stack) {

    if (stack) {
        SAFE_FREE(stack->data);
        stack = NULL;
        return STACK_NO_ERROR;
    }

    log_write("!!!! ERROR: STACK IS NULL --- %s !!!!", __func__);
    fprintf(stderr, "!!!! ERROR: STACK IS NULL --- %s !!!!", __func__);
    return STACK_IS_NULL;
}

void stack_dump(STACK* stack) {

    log_write("\n### ALL INFORMATION ABOUT THE STACK ###\n");
    if (stack) {

        #ifdef STACK_DEBUG
            log_write("STACK NAMED <%s>, BORN AT %s:%d, CALLED FROM %s:%d (%s)",
                       stack->name, stack->born_file_name, stack->born_line, __FILE__, __LINE__, __func__);
        #endif

        log_write("\n--- MAIN INFORMATION ---");
        log_write("\t# STACK SIZE        = %d", stack->data_size);
        log_write("\t# STACK ITEM_NUMBER = %d", stack->item_number);
        log_write("\t# STACK ERROR       = %d\n", stack->error);

        if (stack->data) {

            #ifdef STACK_PROTECTION
                log_write("\n--- PROTECTION MODULE ---");
                log_write("\t# CANARY1       = %lx", stack->canary1);
                log_write("\t# CANARY2       = %lx", stack->canary2);
                log_write("\t# ARRAY_CANARY1 = %lx", *(canary_t*)((char*)stack->data -     sizeof(canary_t)));
                log_write("\t# ARRAY_CANARY2 = %lx", *(canary_t*)((char*)stack->data - 2 * sizeof(canary_t) + stack->data_debug_size_byte));
                log_write("\t# STACK DEBUG SIZE BYTE = %d\n", stack->data_debug_size_byte);
                log_write("\n--- HASH MODULE ---");
                log_write("\t# STACK HASH STRUCT = %x", stack->hash_main);
                log_write("\t# STACK HASH DATA   = %x\n", stack->hash_data);
            #endif

            log_write("\n---STACK_DATA---");

            for (size_t i = 0; i < stack->data_size; i++) {
                log_write("\t# %3d = %7x --- [%p]", i, stack->data[i], stack->data + i);
            }

            log_write("### END OF INFORMATION ABOUT THE STACK ###\n");
        }
        else {
            log_write("# STACK DATA IS NULL POINTER");
        }
    }
    else {
        log_write("# STACK IS NULL POINTER");
    }
}

STACK_ERROR stack_push(STACK* stack, stack_element_t element) {

    STACK_ASSERT(stack);

    if (stack->item_number >= stack->data_size) {

        stack_size_increase(stack);
    }

    stack->data[stack->item_number] = element;
    stack->item_number++;

    ON_PROTECTION(stack_recalc_hash(stack));

    STACK_ASSERT(stack);

    return STACK_NO_ERROR;
}

STACK_ERROR stack_pop(STACK* stack, stack_element_t* pop_to_element) {

    STACK_ASSERT(stack);

    if (stack->item_number == 0) {
        fprintf(stderr, "CAN'T POP ELEMENT, STACK IS EMPTY\n");
        return STACK_UNDERFLOW;
    }

    if (stack->item_number <= stack->data_size/2) {
        stack_size_decrease(stack);
    }

    *pop_to_element = stack->data[--stack->item_number];
    stack->data[stack->item_number] = 0;

    ON_PROTECTION(stack->data[stack->item_number] = POISON);

    ON_PROTECTION(stack_recalc_hash(stack));
    STACK_ASSERT(stack);

    return STACK_NO_ERROR;
}

static STACK_ERROR stack_size_increase(STACK* stack) {

    STACK_ASSERT(stack);

    if (stack->data_size < (size_t)MULT_LIMIT_SIZE) {
        stack_change_size(stack, stack->data_size * 2);
        ON_PROTECTION(stack_fill_poison(stack));
    }
    else {
        stack_change_size(stack, stack->data_size + (size_t)MEGABYTE);
    }

    ON_PROTECTION(stack_recalc_hash(stack));
    STACK_ASSERT(stack);

    return STACK_NO_ERROR;
}

static STACK_ERROR stack_size_decrease(STACK* stack) {

    STACK_ASSERT(stack);

    if (stack->data_size < (size_t)MULT_LIMIT_SIZE) {
        stack_change_size(stack, (size_t)floor(stack->data_size * 0.75));
    }
    else {
        stack_change_size(stack, stack->data_size - (size_t)MEGABYTE);
    }

    ON_PROTECTION(stack_recalc_hash(stack));
    STACK_ASSERT(stack);

    return STACK_NO_ERROR;
}

STACK_ERROR stack_verify(STACK* stack) {

    if (stack == NULL) {
        fprintf(stderr, "!!!! ERROR: STACK_IS_NULL !!!!\n");
        log_write("!!!! ERROR: STACK_IS_NULL !!!!");
        return STACK_IS_NULL;
    }
    if (stack->data == NULL) {
        stack->error = STACK_DATA_IS_NULL;
        fprintf(stderr, "!!!! ERROR: STACK_DATA_IS_NULL !!!!\n");
        log_write("!!!! ERROR: STACK_DATA_IS_NULL !!!!");
        stack_dump(stack);
        return STACK_DATA_IS_NULL;
    }
    if (stack->item_number > stack->data_size) {
        stack->error = STACK_OVERFLOW;
        fprintf(stderr, "!!!! ERROR: STACK_OVER_FLOW !!!!\n");
        log_write("!!!! ERROR: STACK_OVER_FLOW !!!!");
        stack_dump(stack);
        return STACK_OVERFLOW;
    }

    #ifdef STACK_PROTECTION
        if (stack->canary1 != CANARY_HEX_VAL ||
            stack->canary2 != CANARY_HEX_VAL
            || *(canary_t*)((char*)stack->data - sizeof(canary_t)) != CANARY_HEX_VAL ||
            *(canary_t*)((char*)stack->data - 2 * sizeof(canary_t) + stack->data_debug_size_byte) != CANARY_HEX_VAL) {
                stack->error = STACK_BROKEN;
                fprintf(stderr, "!!!! ERROR: STACK BROKEN IN CANARY --- %s:%d !!!!", CALL_FROM);
                log_write("!!!! ERROR: STACK BROKEN IN CANARY --- %s:%d !!!!", CALL_FROM);
                stack_dump(stack);
                return STACK_BROKEN;
        }
        if (stack->hash_main != calc_hash_with_ignore((char*)stack, sizeof(STACK), (char*)(&stack->hash_main), sizeof(stack->hash_main)) ||
            stack->hash_data != calc_hash((char*)stack->data, stack->data_size)) {
            stack->error = STACK_BROKEN;
                fprintf(stderr, "!!!! ERROR: STACK BROKEN IN HASH --- %s:%d !!!!", CALL_FROM);
                log_write("!!!! ERROR: STACK BROKEN IN HASH %s:%d !!!!", CALL_FROM);
                stack_dump(stack);
                return STACK_BROKEN;
        }

    #endif

    return STACK_NO_ERROR;
}

static void stack_change_size(STACK* stack, size_t changed_size) {

#ifdef STACK_PROTECTION
    stack->data_size = changed_size;
    stack->data = (stack_element_t*)((char*)stack->data - sizeof(canary_t));

    size_t temp     = stack->data_size * sizeof(stack_element_t);
    size_t new_size = temp + temp % 8 + 2 * sizeof(canary_t);

    SAFE_RECALLOC(stack->data, stack_element_t, stack->data_debug_size_byte - sizeof(canary_t), new_size);

    stack->data = (stack_element_t*)((char*)stack->data + sizeof(canary_t));
    stack->data_debug_size_byte = new_size;

    memcpy((char*)stack->data + stack->data_debug_size_byte - 2 * sizeof(canary_t), &stack->canary1, sizeof(canary_t));
#else
    size_t old_size  = stack->data_size * sizeof(stack_element_t);
    stack->data_size = changed_size;
    SAFE_RECALLOC(stack->data, stack_element_t, old_size, stack->data_size * sizeof(stack_element_t));
#endif
}

static void stack_data_create(STACK* stack, size_t size) {
#ifdef STACK_PROTECTION
    stack->canary1 = stack->canary2 = CANARY_HEX_VAL;
    size_t offset = (size_t)((size * sizeof(stack_element_t)) % 8);
    size_t debug_size = size * sizeof(stack_element_t) + offset + 2 * sizeof(canary_t);

    SAFE_CALLOC(stack->data, stack_element_t*, debug_size, sizeof(char));

    memcpy(stack->data, &stack->canary1,     sizeof(canary_t));
    memcpy((char*)stack->data + debug_size - sizeof(canary_t), &stack->canary1, sizeof(canary_t));

    stack->data = (stack_element_t*)((char*)stack->data + sizeof(canary_t));
    stack->data_debug_size_byte = debug_size;
#else
    SAFE_CALLOC(stack->data, stack_element_t*, size, sizeof(stack_element_t));
#endif
}

#ifdef STACK_PROTECTION
static void stack_fill_poison(STACK* stack) {

    for (size_t i = stack->item_number; i < stack->data_size; i++) {
        stack->data[i] = POISON;
    }
}

static void stack_recalc_hash(STACK* stack) {

    stack->hash_data = calc_hash((char*)stack->data, stack->data_size);
    stack->hash_main = calc_hash_with_ignore((char*)stack, sizeof(STACK), (char*)(&stack->hash_main), sizeof(stack->hash_main));
}
#endif
