// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libasm.h>
#include <libc.h>
#include <linkedListADT.h>
#include <scheduler.h>
#include <semaphore.h>

typedef struct semaphore_CDT
{
	linked_list_ADT semaphores;  // lo dejo asi x si dsp queremos hacer otra impl
} semaphore_CDT;

typedef struct semaphore
{
	uint32_t id;
	uint32_t value;
	uint16_t attached_processes;
	uint32_t blocked_PIDs[MAX_BLOCKED];
	uint16_t blocked_PIDs_size;
	int mutex;

} semaphore;

semaphore_ADT sem_adt;

static semaphore* new_sem(uint32_t id, uint32_t init_val);
static semaphore* get_sem(uint32_t id);

void
create_semaphore_adt()
{
	sem_adt->semaphores = create_linked_list_ADT();
}

uint32_t
sem_open(uint32_t id, uint32_t init_val)
{
	semaphore* sem = get_sem(id);

	if (sem == NULL) {
		sem = new_sem(id, init_val);
	}

	if (sem->attached_processes >= MAX_BLOCKED) {
		return -1;
	}

	sem->attached_processes++;
	return id;
}

static semaphore*
get_sem(uint32_t id)
{
	

	semaphore* my_sem = NULL;
	semaphore * ret= NULL;

	begin(sem_adt->semaphores);
	while (has_next(sem_adt->semaphores)) {
		my_sem = next(sem_adt->semaphores);

		if (my_sem->id == id) {
			ret= my_sem;
		}
	}

	return ret;
}


static semaphore*
new_sem(uint32_t id, uint32_t val)
{
	semaphore* sem = mm_malloc(sizeof(semaphore));

	sem->value = val;
	sem->id = id;

	sem->attached_processes = 0;
	sem->blocked_PIDs_size = 0;

	sem->mutex = 0;
	append_element(sem_adt->semaphores, sem);
	return sem;
}

int
sem_close(uint32_t id)
{
	semaphore* sem_to_close = get_sem(id);

	if (sem_to_close == NULL) {
		return -1;
	}

	sem_to_close->attached_processes--;

	if (sem_to_close->attached_processes > 0) {
		return 0;
	}

	remove_node_by_data(sem_adt->semaphores, sem_to_close);
	mm_free((void*)sem_to_close);
	
	return 0;
}

int
sem_post(uint32_t id)
{

	semaphore* sem = get_sem(id);
	if (sem == NULL) {
		return -1;
	}

	down(&(sem->mutex));
	if (sem->blocked_PIDs_size > 0) {
		int next_pid = sem->blocked_PIDs[0];
		sem->blocked_PIDs_size--;
		for (int i = 0; i < sem->blocked_PIDs_size; i++) {
			sem->blocked_PIDs[i] = sem->blocked_PIDs[i + 1];
		}
		unblock_process(next_pid);  // marco el proceso como ready
		up(&(sem->mutex));
		return 0;
	} else {
		sem->value++;
	}

	up(&(sem->mutex));

	return 0;
}
int
sem_wait(uint32_t id)
{

	semaphore* sem = get_sem(id);
	if (sem == NULL) {
		return -1;
	}

	down(&(sem->mutex));
	if (sem->value > 0) {
		sem->value--;
		up(&(sem->mutex));
		return 0;
	} else {
		int pid = get_pid();
		sem->blocked_PIDs[sem->blocked_PIDs_size++] = pid;
		up(&(sem->mutex));
		block_process(pid);
	}
	return 0;
}
