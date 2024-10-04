#ifndef SAFE_WRAPPERS_H
#define SAFE_WRAPPERS_H

#include <stdlib.h>
#include <string.h>

#include "log.h"

#define SAFE_CALLOC(ptr, ptr_type, element_num, size_of_element)            \
    do {                                                                    \
        ptr = (ptr_type)calloc(element_num, size_of_element);               \
        if (ptr == NULL) {                                                  \
            log_write("ERROR: CALLOC RETURNED NULL --- %s:%d", CALL_FROM);  \
        }                                                                   \
        log_write("CALLOC --- %s:%d", CALL_FROM);                           \
    } while(0)

#define SAFE_FREE(ptr)                                                      \
    do {                                                                    \
        free(ptr);                                                          \
        ptr = NULL;                                                         \
        log_write("FREE --- %s:%d", CALL_FROM);                             \
    } while(0)

#define SAFE_FOPEN(ptr_to_file, filename, mode)                             \
    do {                                                                    \
        ptr_to_file = fopen(filename, mode);                                \
        if (ptr_to_file == NULL) {                                          \
            log_write("ERROR: CAN'T OPEN THE FILE --- %s:%d", CALL_FROM);   \
        }                                                                   \
        else {                                                              \
            log_write("FOPEN --- %s:%d", CALL_FROM);                        \
        }                                                                   \
    } while(0)

#define SAFE_FCLOSE(ptr_to_file)                                            \
    do {                                                                    \
        if (fclose(ptr_to_file) == EOF) {                                   \
            log_write("ERROR: CAN'T CLOSE THE FILE --- %s:%d", CALL_FROM);  \
        }                                                                   \
        else {                                                              \
            log_write("FCLOSE --- %s:%d", CALL_FROM);                       \
        }                                                                   \
        ptr_to_file = NULL;                                                 \
    } while(0)

//как проверить выделил он память или нет
#define SAFE_RECALLOC(ptr, element_type, old_size_byte, new_size_byte)                  \
    do {                                                                                \
        element_type* temp_ptr = ptr;                                                   \
        log_write("RECALLOC --- %s:%d", CALL_FROM);                                     \
        temp_ptr = (element_type*)realloc(ptr, new_size_byte);                          \
        if (temp_ptr == NULL) {                                                         \
            log_write("ERROR: RECALLOC RETURNS NULL");                                  \
        }                                                                               \
        else {                                                                          \
            ptr = temp_ptr;                                                             \
            if (new_size_byte > old_size_byte) {                                        \
                memset((char*)ptr + old_size_byte, 0, new_size_byte - old_size_byte);   \
            }                                                                           \
        }                                                                               \
    } while(0)

#endif //SAFE_WRAPPERS_H
