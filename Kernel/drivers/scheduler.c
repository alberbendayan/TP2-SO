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

typedef struct scheduler_CDT
{
	node* processes[MAX_PROCESSES];
	linked_list_ADT levels[QTY_READY_LEVELS + 1];
	uint16_t current_pid;
	uint16_t next_unused_pid;
	uint16_t qty_processes;
	int8_t remaining_quantum;
	int8_t kill_fg_process;
} scheduler_CDT;

scheduler_ADT
get_address()
{
	return (scheduler_ADT)SCHEDULER_ADDRESS;
}

scheduler_ADT
create_scheduler()
{
	scheduler_ADT scheduler = (scheduler_ADT)SCHEDULER_ADDRESS;
	for (int i = 0; i < MAX_PROCESSES; i++)
		scheduler->processes[i] = NULL;
	for (int i = 0; i < QTY_READY_LEVELS + 1; i++)
		scheduler->levels[i] = create_linked_list_ADT();
	scheduler->next_unused_pid = 0;
	scheduler->kill_fg_process = 0;
	return scheduler;
}

uint16_t
create_process(main_function code,
               char** args,
               char* name,
               uint8_t priority,
               int16_t file_descriptors[],
               uint8_t unkillable)
{
	scheduler_ADT scheduler = get_address();
	if (scheduler->qty_processes >= MAX_PROCESSES)
		return -1;
	process* proc = (process*)mm_malloc(sizeof(process));
	init_process(proc,
	             scheduler->next_unused_pid,
	             scheduler->current_pid,
	             code,
	             args,
	             name,
	             priority,
	             file_descriptors,
	             unkillable);

	node* process_node;
	if (proc->pid != IDLE_PID)
		process_node = append_element(scheduler->levels[proc->priority], (void*)proc);
	else {
		process_node = mm_malloc(sizeof(node));
		process_node->data = (void*)proc;
	}
	scheduler->processes[proc->pid] = process_node;

	while (scheduler->processes[scheduler->next_unused_pid] != NULL)
		scheduler->next_unused_pid = (scheduler->next_unused_pid + 1) % MAX_PROCESSES;

	scheduler->qty_processes++;
	return proc->pid;
}

uint16_t
getpid()
{
	scheduler_ADT scheduler = get_address();
	return scheduler->current_pid;
}

uint32_t
process_is_alive(uint16_t pid)
{
	scheduler_ADT scheduler = get_address();
	node* process_node = scheduler->processes[pid];
	return process_node != NULL && ((process*)process_node->data)->status != ZOMBIE;
}

void
yield()
{
	scheduler_ADT scheduler = SCHEDULER_ADDRESS;
	scheduler->remaining_quantum = 0;
	// llamar al timertick pra q cambie de procesos
}

uint32_t
set_priority(uint16_t pid, uint8_t new_priority)
{
	scheduler_ADT scheduler = get_address();
	node* n = scheduler->processes[pid];

	if (n == NULL || pid == IDLE_PID)
		return -1;

	process* proc = (process*)n->data;

	if (new_priority < 0 || new_priority >= QTY_READY_LEVELS)
		return -1;

	if (proc->status == READY || proc->status == RUNNING) {
		removeNode(scheduler->levels[proc->priority], n);
		scheduler->processes[proc->pid] = appendNode(scheduler->levels[new_priority], n);
	}

	proc->priority = new_priority;
	return proc->priority;
}

int8_t
set_status(uint16_t pid, uint8_t new_status)
{
	scheduler_ADT scheduler = get_address();
	node* my_node = scheduler->processes[pid];

	if (my_node == NULL || pid == IDLE_PID)
		return -1;

	process* proc = (process*)my_node->data;
	process_status old_status = proc->status;

	// Verifica si el nuevo estado es válido
	if (new_status == ZOMBIE || old_status == ZOMBIE) {
		return -1;
	}

	// No se permite cambiar a RUNNING desde otro estado
	if (new_status == RUNNING && old_status != READY) {
		return -1;
	}
	if (new_status == proc->status)
		return new_status;

	if (new_status == BLOCKED) {
		removeNode(scheduler->levels[proc->priority], my_node);
		appendNode(scheduler->levels[BLOCKED_INDEX], my_node);
	} else if (old_status == BLOCKED) {
		removeNode(scheduler->levels[BLOCKED_INDEX], my_node);
		proc->priority = MAX_PRIORITY;
		prependNode(scheduler->levels[proc->priority], my_node);
		scheduler->remaining_quantum = 0;
	}

	proc->status = new_status;

	return proc->status;
}

process_status
get_process_status(uint16_t pid)
{
	scheduler_ADT scheduler = get_address();
	node* my_node = scheduler->processes[pid];
	if (my_node == NULL) {
		return DEAD;
	}
	return ((process*)my_node->data)->status;
}

void*
schedule(void* prev_stack_pointer)
{
	scheduler_ADT scheduler = get_address();  // falta
}

int32_t
kill_current_process(int32_t ret_value)
{
	scheduler_ADT scheduler = get_address();
	return kill_process(scheduler->current_pid, ret_value);
}

int32_t
killProcess(uint16_t pid, int32_t ret_value)
{
	scheduler_ADT scheduler = get_address();
	node* process_to_kill_node = scheduler->processes[pid];
	if (process_to_kill_node == NULL)
		return -1;
	process* process_to_kill = (process*)process_to_kill_node->data;
	if (process_to_kill->status == ZOMBIE || process_to_kill->unkillable)
		return -1;

	closeFileDescriptors(process_to_kill);

	uint8_t priorityIndex = process_to_kill->status != BLOCKED ? process_to_kill->priority : BLOCKED_INDEX;
	removeNode(scheduler->levels[priorityIndex], process_to_kill_node);
	process_to_kill->ret_value = ret_value;

	process_to_kill->status = ZOMBIE;

	begin(process_to_kill->zombie_children);
	while (hasNext(process_to_kill->zombie_children)) {
		destroyZombie(scheduler, (process*)next(process_to_kill->zombie_children));
	}

	node* parent_node = scheduler->processes[process_to_kill->parent_pid];
	if (parent_node != NULL && ((process*)parent_node->data)->status != ZOMBIE) {
		process* parent = (process*)parent_node->data;
		appendNode(parent->zombie_children, process_to_kill_node);
		if (processIsWaiting(parent, process_to_kill->pid))
			setStatus(process_to_kill->parent_pid, READY);
	} else {
		destroyZombie(scheduler, process_to_kill);
	}
	if (pid == scheduler->current_pid)
		yield();
	return 0;
}

int8_t
changeFD(uint16_t pid, uint8_t position, int16_t new_fd)
{
	scheduler_ADT scheduler = get_address();
	node* process_node = scheduler->processes[pid];
	if (pid == IDLE_PID || process_node == NULL)
		return -1;
	process* proc = (process*)process_node->data;
	proc->file_descriptors[position] = new_fd;
	return 0;
}