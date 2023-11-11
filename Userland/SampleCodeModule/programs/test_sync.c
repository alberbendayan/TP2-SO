#include "my_syscall.h"
#include "test_util.h"

#include <stdint.h>
#include <tests.h>

#define NULL (void*)0
#define SEM_ID 98
#define TOTAL_PAIR_PROCESSES 2

int64_t global;  // shared memory

void
slowInc(int64_t* p, int64_t inc)
{
	uint64_t aux = *p;
	my_yield();
	aux += inc;
	*p = aux;
}

uint64_t
my_process_inc(uint64_t argc, char* argv[])
{
	uint64_t n;
	int8_t inc;
	int8_t use_sem;

	if (argc != 3) {
		puts("return -1\n", 0xff0000);
		return -1;
	}

	if ((n = satoi(argv[0])) <= 0) {
		puts("return -1 argv[0]\n", 0xff0000);
		return -1;
	}
	if ((inc = satoi(argv[1])) == 0) {
		puts("return -1 argv[1]\n", 0xff0000);
		return -1;
	}
	if ((use_sem = satoi(argv[2])) < 0) {
		puts("return -1 argv[2]\n", 0xff0000);
		return -1;
	}

	if (use_sem) {
		if (!my_sem_open(SEM_ID, 1)) {
			puts("test_sync: ERROR opening semaphore\n");
			return -1;
		}
	}
	uint64_t i;
	for (i = 0; i < n; i++) {
		if (use_sem)
			my_sem_wait(SEM_ID);
		slowInc(&global, inc);
		if (use_sem)
			my_sem_post(SEM_ID);
	}

	if (use_sem)
		my_sem_close(SEM_ID);

	my_kill(my_getpid());
	return 0;
}

uint64_t
test_sync(uint64_t argc, char* argv[])
{  //{n, use_sem, 0}
	uint64_t pids[2 * TOTAL_PAIR_PROCESSES];
	if (argc != 3) {
		puts("Return -1\n", 0xff0000);
		return -1;
	}
	char* argvDec[] = { argv[1], "-1", argv[2], NULL };
	char* argvInc[] = { argv[1], "1", argv[2], NULL };

	global = 0;

	uint64_t i;
	for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
		pids[i] = my_create_process(my_process_inc, 3, argvDec);
		pids[i + TOTAL_PAIR_PROCESSES] = my_create_process(my_process_inc, 3, argvInc);
	}

	for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
		my_wait(pids[i]);
		my_wait(pids[i + TOTAL_PAIR_PROCESSES]);
	}

	puts("Final value: ", 0xff0000);
	char c[100];
	uint_to_base(global, c, 10);
	puts(c, 0xff0000);
	puts("\n", 0xff0000);
	return 0;
}
