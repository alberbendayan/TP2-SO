// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <memoryManagement.h>
#include <memoryManagementADT.h>

void *mem_start;
unsigned int mem_size;

static memory_managment_ADT memory_manager;

void mm_init(void *mem_for_mm, void* const restrict start){
	memory_manager=create_mm(mem_for_mm, start);
}

void *mm_malloc(uint64_t mem_to_alloc_size){
	return mem_alloc(memory_manager, mem_to_alloc_size);
}

void mm_free(void *mem_block){
	return free_mem(memory_manager,mem_block);
}
uint64_t mm_heap_size(){
	return heap_size();
}

uint64_t mm_heap_left(){
	return heap_left(memory_manager);
}

uint64_t mm_used_heap(){
	return used_heap(memory_manager);
}
void mm_status(unsigned int* status){
	status[0] = mm_heap_size();
	status[1] = mm_heap_left();
	status[2] = mm_used_heap();
}



