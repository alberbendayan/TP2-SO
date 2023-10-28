#ifndef _PROCESS_H
#define _PROCESS_H
#include <linkedListADT.h>

#define BUILT_IN_DESCRIPTORS 3


typedef enum { BLOCKED = 0,
			   READY,
			   RUNNING,
			   ZOMBIE,
			   DEAD } process_status;

typedef struct Process { 
    uint16_t pid;
    uint16_t parent_pid;
    uint16_t waiting_for_pid;
    void *stack_base; // MemoryBlock
    void *stack_pos;
    char **argv;
    char *name;
    uint8_t unkillable;
    uint8_t priority;
    process_status status;
    int16_t file_descriptors[BUILT_IN_DESCRIPTORS];
    int32_t ret_value;
    linked_list_ADT zombie_children;
} Process;




#endif