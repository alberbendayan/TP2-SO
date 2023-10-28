#ifdef LINEAR
#include <memoryManagementADT.h>

typedef struct mem_block {
  struct mem_block *next_mem_block;
  unsigned int block_size;
} mem_block;

typedef struct memory_managment_CDT {
  mem_block start;
  mem_block end;
  unsigned int free_bytes_remaining;
} memory_managment_CDT;

static const uint16_t STRUCT_SIZE =
    ((sizeof(mem_block) + (BYTE_ALIGMENT - 1)) & ~MASK_BYTE_ALIGMENT);

#define MINIMUM_BLOCK_SIZE ((unsigned int)(STRUCT_SIZE * 2))

static void insert_into_free_list(memory_managment_ADT memory_manager,mem_block *block_to_insert);

unsigned int heap_size() { return TOTAL_HEAP_SIZE; }

unsigned int heap_left(memory_managment_ADT memory_manager) {
  return memory_manager->free_bytes_remaining;
}

unsigned int used_heap(memory_managment_ADT memory_manager) {
  return TOTAL_HEAP_SIZE - heap_left(memory_manager);
}

memory_managment_ADT create_mm(
    void *const restrict mem_for_mm,
    void *const restrict start) {
  memory_managment_ADT memory_managment =
      (memory_managment_ADT)mem_for_mm;
  memory_managment->free_bytes_remaining = TOTAL_HEAP_SIZE;

  mem_block *starting_block = (void *)start;

  memory_managment->start.next_mem_block = (void *)starting_block;
  memory_managment->start.block_size = (unsigned int)0;

  memory_managment->end.next_mem_block = NULL;
  memory_managment->end.block_size = TOTAL_HEAP_SIZE;

  starting_block->block_size = TOTAL_HEAP_SIZE;
  starting_block->next_mem_block = &memory_managment->end;

  return memory_managment;
}

static void insert_into_free_list(memory_managment_ADT memory_manager,
                                    mem_block *block_to_insert) {
  mem_block *iterator = &memory_manager->start;
  unsigned int block_size_to_insert = block_to_insert->block_size;

  while (iterator->next_mem_block->block_size < block_size_to_insert) {
    iterator = iterator->next_mem_block;
  }

  block_to_insert->next_mem_block = iterator->next_mem_block;
  iterator->next_mem_block = block_to_insert;
}

void *mem_alloc(memory_managment_ADT const memory_manager, unsigned int mem_to_allocate){
  
  mem_block *current_block;
  void *blockToReturn = NULL;

  if (mem_to_allocate == 0) {
    return NULL;
  }
  mem_to_allocate += STRUCT_SIZE;

  // alineamos los bytes
  if ((mem_to_allocate & MASK_BYTE_ALIGMENT) != 0) {
    mem_to_allocate += (BYTE_ALIGMENT - (mem_to_allocate & MASK_BYTE_ALIGMENT));
  }

  if (mem_to_allocate < TOTAL_HEAP_SIZE) {
    mem_block *previous_block = &memory_manager->start;
    current_block = memory_manager->start.next_mem_block;

    while ((current_block->block_size < mem_to_allocate) &&
           (current_block->next_mem_block != NULL)) {
      previous_block = current_block;
      current_block = current_block->next_mem_block;
    }
    if (current_block == &memory_manager->end) {
      return NULL;
    }

    blockToReturn =
        (void *)(((uint8_t *)previous_block->next_mem_block) + STRUCT_SIZE);

    previous_block->next_mem_block = current_block->next_mem_block;

    if ((current_block->block_size - mem_to_allocate) > MINIMUM_BLOCK_SIZE) {
      mem_block *newBlock = (void *)(((uint8_t *)current_block) + mem_to_allocate);

      newBlock->block_size = current_block->block_size - mem_to_allocate;
      current_block->block_size = mem_to_allocate;

      insert_into_free_list(memory_manager, newBlock);
    }

    memory_manager->free_bytes_remaining -= current_block->block_size;
  }

  return blockToReturn;


}

void free_mem(memory_managment_ADT const memory_manager, void *block) {
  if (block == NULL) {
    return;
  }

  uint8_t *mem_to_free = ((uint8_t *)block);
  mem_block *block_to_free;

  mem_to_free -= STRUCT_SIZE;

  block_to_free = (void *)mem_to_free;

  insert_into_free_list(memory_manager, ((mem_block *)block_to_free));
  memory_manager->free_bytes_remaining += block_to_free->block_size;
}
#endif