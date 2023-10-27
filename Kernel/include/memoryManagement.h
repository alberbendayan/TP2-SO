#ifndef MEMORYMANAGEMENT_H
#define MEMORYMANAGEMENT_H

#include <stdint.h>

#define HEAPSIZE (1024 * 1024 * 128) /*128MB*/
#define NULL ((void *)0)

void mm_init(void *init_address, void* const restrict size);

void *mm_malloc(uint64_t size);

void mm_free(void *ptr);

void mem_status(unsigned int *status);

#endif