#include <memoryManagement.h>
#include <memoryManagementTAD.h>

void *mem_start;
unsigned int mem_size;

static memory_managment_ADT memory_manager;

void mm_init(void *mem_for_mm, unsigned int start){
	memory_manager=create_mm(mem_for_mm, start);
}

void *mm_malloc(uint64_t mem_to_alloc_size){
	return mem_alloc(memory_manager, mem_to_alloc_size);
}

void mm_free(void *mem_block){
	free_mem(memory_manager,mem_block);
}

void mem_status(unsigned int* status){
	status[0] = heap_size();
	status[1] = heap_left(memory_manager);
	status[2] = used_heap(memory_manager);
}

