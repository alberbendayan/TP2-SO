// #ifdef BUDDY

#include <MemoryManagementTAD.h>
#include <stdint.h>

#define MINIMUM_BLOCK_SIZE 512

typedef struct mem_block
{
	unsigned char free;
	int history;
	struct mem_block* next_block;
	unsigned int size;
} mem_block;

typedef struct memory_managment_CDT
{
	mem_block start;
	mem_block end;
	unsigned int freeBytesRemaining;
} memory_managment_CDT;



unsigned int
heap_size()
{
	return TOTAL_HEAP_SIZE;
}

unsigned int
heap_left(memory_managment_ADT memory_manager)
{
	return memory_manager->free_bytes_remaining;
}

unsigned int
used_heap(memory_managment_ADT memory_manager)
{
	return TOTAL_HEAP_SIZE - heap_left(memory_manager);
}

memory_managment_ADT
create_MM(void* const restrict mem_for_memory_managment, void* const restrict managed_mem)
{
}
static void
insert_into_free_list(memory_managment_ADT memory_manager, mem_block* block_to_insert)
{
}
void*
mem_alloc(memory_managment_ADT const memory_manager, unsigned int mem_to_allocate)
{
}
void
free_mem(memory_managment_ADT const memory_manager, void* block)
{
}


// #endif