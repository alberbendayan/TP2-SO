#include <linkedListADT.h>
#include <memoryManagement.h>
#include <scheduler.h>
#include <stdlib.h>
#include <video.h>

#define QTY_READY_LEVELS 5
#define MAX_PRIORITY 4
#define MIN_PRIORITY 0
#define BLOCKED_INDEX QTY_READY_LEVELS
#define MAX_PROCESSES 4096
#define IDLE_PID 0
#define QUANTUM_COEF 2
#define SCHEDULER_ADDRESS 0x60000

typedef struct scheduler_CDT {
	node *processes[MAX_PROCESSES];
	linked_list_ADT levels[QTY_READY_LEVELS + 1];
	uint16_t current_pid;
	uint16_t next_unused_pid;
	uint16_t qty_processes;
	int8_t remaining_quantum;
	int8_t kill_fg_process;
} scheduler_CDT;

scheduler_ADT create_scheduler() {
	scheduler_ADT scheduler = (scheduler_ADT) SCHEDULER_ADDRESS;
	for (int i = 0; i < MAX_PROCESSES; i++)
		scheduler->processes[i] = NULL;
	for (int i = 0; i < QTY_READY_LEVELS + 1; i++)
		scheduler->levels[i] = create_linked_list_ADT();
	scheduler->next_unused_pid = 0;
	scheduler->kill_fg_process = 0;
	return scheduler;
}

uint16_t create_process(main_function code, char **args, char *name, uint8_t priority, int16_t file_descriptors[], uint8_t unkillable) {
	scheduler_ADT scheduler = SCHEDULER_ADDRESS;
	if (scheduler->qty_processes >= MAX_PROCESSES)
		return -1;
	process *proc = (process *) mm_malloc(sizeof(process));
	init_process(proc, scheduler->next_unused_pid, scheduler->current_pid, code, args, name, priority, file_descriptors, unkillable);

	node *process_node;
	if (proc->pid != IDLE_PID)
		process_node = append_element(scheduler->levels[proc->priority], (void *) proc);
	else {
		process_node = mm_malloc(sizeof(node));
		process_node->data = (void *) proc;
	}
	scheduler->processes[proc->pid] = process_node;

	while (scheduler->processes[scheduler->next_unused_pid] != NULL)
		scheduler->next_unused_pid = (scheduler->next_unused_pid + 1) % MAX_PROCESSES;
	
	scheduler->qty_processes++;
	return proc->pid;
}

uint16_t getpid() {
	scheduler_ADT scheduler = SCHEDULER_ADDRESS;
	return scheduler->current_pid;
}

uint32_t process_is_alive(uint16_t pid) {
	scheduler_ADT scheduler = SCHEDULER_ADDRESS;
	node *process_node = scheduler->processes[pid];
	return process_node != NULL && ((process *) process_node->data)->status != ZOMBIE;
}

void yield() {
	scheduler_ADT scheduler = SCHEDULER_ADDRESS;
	scheduler->remaining_quantum = 0;
	// llamar al timertick pra q cambie de procesos
}