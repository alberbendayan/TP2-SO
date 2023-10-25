#ifndef MEMORY_MANAGEMENT_ADT_H
#define MEMORY_MANAGEMENT_ADT_H

#include <stdint.h>

#define NULL ((void *)0)
#define BYTE_ALIGMENT 8
#define MASK_BYTE_ALIGMENT 0x07
#define TOTAL_HEAP_SIZE 1024*1024*128 // 128MB
typedef struct memory_managment_CDT *memory_managment_ADT;

memory_managment_ADT create_MM(void *const restrict mem_for_memory_managment, void *const restrict managed_mem);
static void insert_into_free_list(memory_managment_ADT memory_manager,mem_block *block_to_insert);
void *mem_alloc(memory_managment_ADT const memory_manager, unsigned int mem_to_allocate);
void free_mem(memory_managment_ADT const memory_manager, void *block);
unsigned int heap_size();
unsigned int heap_left(memory_managment_ADT memory_manager);
unsigned int used_heap(memory_managment_ADT memory_manager);


#endif