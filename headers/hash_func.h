#ifndef HASH_FUNC_H
#define HASH_FUNC_H

int calc_hash(char* ptr, size_t n_byte);

int calc_hash_with_ignore(char* ptr, size_t n_byte, char* ptr_to_ignore, size_t n_byte_to_ignore);

#endif //HASH_FUNC_H
