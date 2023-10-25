// #ifdef BUDDY

#include <MemoryManagementTAD.h>
#include <stdint.h>

#define MINIMUM_BLOCK_SIZE 512

typedef struct mem_block
{
	unsigned char free;
	int history;
	struct mem_block* next_mem_block;
	unsigned int size;
} mem_block;

typedef struct memory_managment_CDT
{
	mem_block start;
	mem_block end;
	unsigned int free_bytes_remaining;
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
// muy parecido a la otra implementacion, solo le agrego el history y free
memory_managment_ADT
create_MM(void* const restrict mem_for_memory_managment, void* const restrict managed_mem)
{
	memory_managment_ADT memoryManagment = (memory_managment_ADT)mem_for_memory_managment;
	memoryManagment->free_bytes_remaining = TOTAL_HEAP_SIZE;

	mem_block* startingBlock = (void*)managed_mem;

	memoryManagment->start.next_mem_block = (void*)startingBlock;
	memoryManagment->start.size = (unsigned int)0;

	memoryManagment->end.next_mem_block = NULL;
	memoryManagment->end.size = TOTAL_HEAP_SIZE;

	startingBlock->size = TOTAL_HEAP_SIZE;
	startingBlock->next_mem_block = &memoryManagment->end;

	startingBlock->free = 1;
	startingBlock->history = 0;

	return memoryManagment;
}

static int
insert_block_as_free(memory_managment_ADT mm, mem_block* block_insert, unsigned char merge)
{
	if (block_insert->size == TOTAL_HEAP_SIZE) {
		mm->start.next_mem_block = block_insert;
		block_insert->next_mem_block = &mm->end;
		block_insert->free = 1;
		return block_insert->size;
	}

	mem_block* buddy = NULL;
	int auxMerge = 0;

	if (merge) {
		if ((block_insert->history & 0x1) != 0) {  
			buddy = (mem_block*)((uint64_t)block_insert - block_insert->blockSize);
			
            if (buddy->free && buddy->size == block_insert->size) {
				buddy->size *= 2;
				auxMerge = 1;
				block_insert = buddy;
			}
            
		} else {
			buddy = (mem_block*)((uint64_t)block_insert + block_insert->blockSize);
			
            if (buddy->free && buddy->size == block_insert->size) {
				block_insert->size *= 2;
				auxMerge = 1;
			}
		}
	}

	if (auxMerge == 1) {
		block_insert->history = block_insert->history >> 1;
		remove_block_as_free(mm, buddy);
		return insert_block_as_free(mm, block_insert, 1);
	}

	mem_block* iter = &mm->start;
	int blockSize = block_insert->size;

	while (iter->next_mem_block->size < blockSize) {
		iter = iter->next_mem_block;
	}

	block_insert->next_mem_block = iter->next_mem_block;
	iter->next_mem_block = block_insert;
	block_insert->free = 1;
	return block_insert->size;
}

static void remove_block_as_free(memory_managment_ADT mm, mem_block* block_delete) {
  mem_block *iter = &mm->start;

  while (iter != NULL && iter->next_mem_block != block_delete) {
    iter = iter->next_mem_block;
  }
  if (iter != NULL) {
    iter->next_mem_block = iter->next_mem_block->next_mem_block;
  }
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