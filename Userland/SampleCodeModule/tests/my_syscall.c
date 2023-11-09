#include <stdint.h>
#include <syscalls.h>

typedef int (*main_function)(int argc, char** args);

typedef struct process_initialization
{
	main_function code;
	char** args;
	char* name;
	uint8_t priority;
	int16_t* file_descriptors;
	uint8_t unkillable;
} process_initialization;

int64_t
my_getpid()
{
	return asm_get_current_id();
}

int64_t
my_create_process(char* name, uint64_t argc, char* argv[])
{
  process_initialization p;
  p.args=argv;
  p.name=name;
  p.code=(*main_function)(argc,argv);
  p.file_descriptors=[0,1,2];
  p.unkillable=0;
  p.priority=4;
	return asm_init_process(&p);
}

int64_t
my_nice(uint64_t pid, uint64_t newPrio)
{
	return asm_nice(pid, newPrio);
}

int64_t
my_kill(uint64_t pid)
{
	return asm_kill_process(pid, 0);
}

int64_t
my_block(uint64_t pid)
{
	return asm_block_process(int pid);
}

int64_t
my_unblock(uint64_t pid)
{
	return asm_unblock_process(int pid);
}

int64_t
my_sem_open(uint32_t sem_id, uint32_t initialValue)
{
	return asm_sem_open(sem_id, initialValue);
}

int64_t
my_sem_wait(uint32_t sem_id)
{
	return asm_sem_wait(sem_id);
}

int64_t
my_sem_post(uint32_t sem_id)
{
	return asm_sem_post(sem_id);
}

int64_t
my_sem_close(uint32_t sem_id)
{
	return asm_sem_close(sem_id);
}

int64_t
my_yield()
{
	return asm_yield();
}

int64_t
my_wait(int64_t pid)
{
	return asm_waiting_for_pid(my_getpid(),pid);
}
