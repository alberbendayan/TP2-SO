#ifdef BUDDY

#include <memoryManagementADT.h>
#include <stdint.h>

#define MINIMUM_BLOCK_SIZE 512

typedef struct mem_block
{
	unsigned char free;
	int history;
	struct mem_block* next_mem_block;
	unsigned int block_size;
} mem_block;

typedef struct memory_managment_CDT
{
	mem_block start;
	mem_block end;
	unsigned int free_bytes_remaining;
} memory_managment_CDT;

static const uint16_t STRUCT_SIZE = ((sizeof(mem_block) + (BYTE_ALIGMENT - 1)) & ~MASK_BYTE_ALIGMENT);
static void remove_block_as_free(memory_managment_ADT mm, mem_block* block_delete);

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
create_mm(void* const restrict mem_for_memory_managment, void* const restrict managed_mem)
{
	memory_managment_ADT mm = (memory_managment_ADT)mem_for_memory_managment;
	mm->free_bytes_remaining = TOTAL_HEAP_SIZE;

	mem_block* start = (void*)managed_mem;

	mm->start.next_mem_block = (void*)start;
	mm->start.block_size = (unsigned int)0;

	mm->end.next_mem_block = NULL;
	mm->end.block_size = TOTAL_HEAP_SIZE;

	start->block_size = TOTAL_HEAP_SIZE;
	start->next_mem_block = &mm->end;

	start->free = 1;
	start->history = 0;

	return mm;
}

static int
insert_block_as_free(memory_managment_ADT mm, mem_block* block_insert, unsigned char merge)
{
	if (block_insert->block_size == TOTAL_HEAP_SIZE) {
		mm->start.next_mem_block = block_insert;
		block_insert->next_mem_block = &mm->end;
		block_insert->free = 1;
		return block_insert->block_size;
	}

	mem_block* buddy = NULL;
	int aux = 0;

	if (merge) {
		if ((block_insert->history & 0x1) != 0) {
			buddy = (mem_block*)((uint64_t)block_insert - block_insert->block_size);

			if (buddy->free && buddy->block_size == block_insert->block_size) {
				buddy->block_size *= 2;
				aux = 1;
				block_insert = buddy;
			}

		} else {
			buddy = (mem_block*)((uint64_t)block_insert + block_insert->block_size);

			if (buddy->free && buddy->block_size == block_insert->block_size) {
				block_insert->block_size *= 2;
				aux = 1;
			}
		}
	}

	if (aux == 1) {
		block_insert->history = block_insert->history >> 1;
		remove_block_as_free(mm, buddy);
		return insert_block_as_free(mm, block_insert, 1);
	}

	mem_block* iter = &mm->start;
	int size = block_insert->block_size;

	while (iter->next_mem_block->block_size < size) {
		iter = iter->next_mem_block;
	}

	block_insert->next_mem_block = iter->next_mem_block;
	iter->next_mem_block = block_insert;
	block_insert->free = 1;
	return block_insert->block_size;
}

static void
remove_block_as_free(memory_managment_ADT mm, mem_block* block_delete)
{
	mem_block* iter = &mm->start;

	while (iter != NULL && iter->next_mem_block != block_delete) {
		iter = iter->next_mem_block;
	}
	if (iter != NULL) {
		iter->next_mem_block = iter->next_mem_block->next_mem_block;
	}
}

void*
mem_alloc(memory_managment_ADT const mm, unsigned int mem_to_allocate)
{
	mem_block *current, *previous;
	void* ret = NULL;

	if (mem_to_allocate == 0) {
		return NULL;
	}
	// Increase size so that it can contain a MemBlock
	mem_to_allocate += STRUCT_SIZE;

	// Byte aligment
	if ((mem_to_allocate & MASK_BYTE_ALIGMENT) != 0) {
		mem_to_allocate += (BYTE_ALIGMENT - (mem_to_allocate & MASK_BYTE_ALIGMENT));
	}

	if (mem_to_allocate > TOTAL_HEAP_SIZE) {
		return NULL;
	}

	previous = &mm->start;
	current = mm->start.next_mem_block;

	while ((current->block_size < mem_to_allocate) && (current->next_mem_block != NULL)) {
		previous = current;
		current = current->next_mem_block;
	}

	if (current == &mm->end) {
		return NULL;
	}
	ret = (void*)(((uint8_t*)previous->next_mem_block) + STRUCT_SIZE);

	previous->next_mem_block = current->next_mem_block;

	while (current->block_size / 2 >= MINIMUM_BLOCK_SIZE && current->block_size / 2 >= mem_to_allocate) {
		current->block_size /= 2;
		current->history = current->history << 1;
		mem_block* new = (void*)(((uint64_t)current) + current->block_size);
		new->block_size = current->block_size;
		new->free = 1;
		new->history = current->history | 0x1;  // marco el bloque derecho
		insertBlockIntoFreeList(mm, new, 0);
	}

	mm->free_bytes_remaining -= current->block_size;
	current->free = 0;

	return ret;
}

void
free_mem(memory_managment_ADT const mm, void* block)
{
	if (block == NULL) {
		return;
	}
	uint8_t* mem_free = ((uint8_t*)block);
	mem_block* block_free;

	mem_free -= STRUCT_SIZE;

	block_free = (void*)mem_free;

	unsigned int aux = block_free->block_size;

	insertBlockIntoFreeList(mm, ((mem_block*)block_free), 1);
	mm->free_bytes_remaining += aux;
}
#endif