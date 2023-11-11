#ifndef _PROCESS_H
#define _PROCESS_H
#include <linkedListADT.h>

#define BUILT_IN_DESCRIPTORS 3

typedef enum
{
	BLOCKED = 0,
	READY,
	RUNNING,
	DEAD
} process_status;

/* File Descriptors*/
#define DEV_NULL -1

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define READ 0
#define WRITE 1
#define ERROR 2
typedef struct process
{
	uint16_t pid;
	uint16_t parent_pid;
	uint16_t waiting_for_pid;
	void* stack_base; 
	void* stack_pos;
	char** argv;
	char* name;
	uint8_t unkillable;
	uint8_t priority;
	process_status status;
	int file_descriptors[BUILT_IN_DESCRIPTORS];
	int32_t ret_value;
} process;

typedef int (*main_function)(int argc, char** args);

typedef struct process_snapshot
{
	uint16_t pid;
	uint16_t parent_pid;
	void* stack_base;
	void* stack_pos;
	char* name;
	uint8_t priority;
	process_status status;
	uint8_t foreground;
} process_snapshot;

typedef struct process_initialization
{
	main_function code;
	char** args;
	char* name;
	uint8_t priority;
	int* file_descriptors;
	uint8_t unkillable;
} process_initialization;

void init_process(process* proc,
                  uint16_t pid,
                  uint16_t parent_pid,
                  main_function code,
                  char** args,
                  char* name,
                  uint8_t priority,
                  int file_descriptors[],
                  uint8_t unkillable);

int load_snapshot(process_snapshot* snapshot, process* proc);
int process_is_waiting(process* proc, uint16_t pid_to_wait);
void free_process(process* proc);
void close_file_descriptors(process* proc);

#endif