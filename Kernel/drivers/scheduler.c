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
