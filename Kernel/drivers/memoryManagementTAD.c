#include <memoryManagementTAD.h>

typedef struct MemBlock {
  struct MemBlock *nextMemBlock;
  unsigned int blockSize;
} MemBlock;

typedef struct MemoryManagmentCDT {
  MemBlock start;
  MemBlock end;
  unsigned int freeBytesRemaining;
} MemoryManagmentCDT;

static const uint16_t STRUCT_SIZE =
    ((sizeof(MemBlock) + (BYTE_ALIGMENT - 1)) & ~MASK_BYTE_ALIGMENT);

#define MINIMUM_BLOCK_SIZE ((unsigned int)(STRUCT_SIZE * 2))


unsigned int heapSize() { return TOTAL_HEAP_SIZE; }

unsigned int heapLeft(MemoryManagmentADT memoryManager) {
  return memoryManager->freeBytesRemaining;
}

unsigned int usedHeap(MemoryManagmentADT memoryManager) {
  return TOTAL_HEAP_SIZE - heapLeft(memoryManager);
}