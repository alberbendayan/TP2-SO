#include <linkedListADT.h>
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

	begin(sem_adt->semaphores);
	while (has_next(sem_adt->semaphores)) {
		my_sem = next(sem_adt->semaphores);
		if (my_sem->id == id) {
			return my_sem;
		}
	}

	return my_sem;
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

	remove_node(sem_adt->semaphores, sem_to_close);
	mm_free(sem_to_close);
	return 0;
}

int
sem_wait(uint32_t id)
{
	semaphore* sem = get_sem(id);

	return 0;
}
int
sem_post(uint32_t id)
{
	semaphore* sem = get_sem(id);
    
	return 0;
}
