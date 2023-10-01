#ifndef LIBC_H
#define LIBC_H

#include <stdint.h>

#define DEC 10
#define HEX 16
#define BIN 2
#define OCT 8

// memory management
void* memset(void* destination, int32_t character, uint64_t length);
void* memcpy(void* destination, const void* source, uint64_t length);

// string manipulation
uint32_t strlen(char* str);
uint32_t uint_to_base(uint64_t value, char* buff, uint32_t base);

#endif
