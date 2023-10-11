#ifndef MEMORYMANAGEMENT_H
#define MEMORYMANAGEMENT_H

#include <stdint.h>

#define HEAPSIZE (1024 * 1024 * 128) /*128MB*/
#define NULL ((void *)0)

void mmInit(void *init_address, unsigned int size);

void *mmMalloc(uint64_t size);

void mmFree(void *ptr);

void fillMemInfo(char *buffer);

#endif