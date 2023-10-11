#include <memoryManagement.h>

void *mem_start;
unsigned int mem_size;

/*typedef struct memory_manager_CDT {
	char *nextAddress;
} memory_manager_CDT;*/

void mmInit(void *init_address, unsigned int size){
	mem_start = init_address;
	mem_size = size;
}

void *mmMalloc(uint64_t size){
	
}

void mmFree(void *ptr){}

void fillMemInfo(char *buffer){}

/*MemoryManagerADT createMemoryManager(void *const restrict memoryForMemoryManager, void *const restrict managedMemory) {
	MemoryManagerADT memoryManager = (MemoryManagerADT) memoryForMemoryManager;
	memoryManager->nextAddress = managedMemory;
	
	return memoryManager;
}

void *allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate) {
	char *allocation = memoryManager->nextAddress;

	memoryManager->nextAddress += memoryToAllocate;

	return (void *) allocation;
}*/
