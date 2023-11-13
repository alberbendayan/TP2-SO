// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <syscalls.h>

int64_t
my_getpid()
{
	return asm_get_current_id();
}

int64_t
my_create_process(void * code, uint64_t argc, char* argv[])
{
	int fd[3] = { 0, 1, 2 };
	process_initialization p;
	p.args = argv;
	p.name = argv[0];
	p.code = code;
	p.file_descriptors = fd;
	p.unkillable = 0;
	p.priority = 4;
	return asm_init_process(&p);
}

int64_t
my_nice(uint64_t pid, uint64_t newPrio)
{
	return asm_set_priority(pid, newPrio);
}

int64_t
my_kill(uint64_t pid)
{
	return asm_kill_process(pid, 0);
}

int64_t
my_block(uint64_t pid)
{
	return asm_block_process(pid);
}

int64_t
my_unblock(uint64_t pid)
{
	return asm_unblock_process(pid);
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
	return asm_wait_pid(pid);
}

int64_t
my_malloc(int64_t size){
	return (int64_t) asm_malloc(size);
}