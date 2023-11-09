#include <libasm.h>
#include <phylos.h>
#include <shell.h>
#include <stdlib.h>
#include <syscalls.h>

#define NULL (void*)0
#define N 5
#define THINKING 2
#define HUNGRY 1
#define EATING 0
#define LEFT (phnum + n - 1) % n
#define RIGHT (phnum + 1) % n
#define GENERALSEMID 100
#define MAX_PHILOSOPHERS 10

void print_table();

int state[MAX_PHILOSOPHERS];
int pids[MAX_PHILOSOPHERS];
int n = N;

int mutex;
int chopsticks[N];

uint32_t
phylo_init()
{
	puts("Hola soy el filosofo\n", 0xff0000);
	philosophers_program();
}

void
test(int phnum)
{
	if (state[phnum] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
		state[phnum] = EATING;
		print_table();
		asm_sem_post(chopsticks[phnum]);
	}
}

void
take_fork(int phnum)
{
	asm_sem_wait(mutex);

	state[phnum] = HUNGRY;

	test(phnum);

	asm_sem_post(mutex);

	asm_sem_wait(chopsticks[phnum]);

	asm_sleep(1);
}

void
put_fork(int phnum)
{
    asm_sem_wait(mutex);
    state[phnum] = THINKING;
	test(LEFT);
	test(RIGHT);
	asm_sem_post(mutex);
}

void*
philosopher(int num)
{
	puts("soy el filosofo ", 0xff0000);
	putchar(num + '0', 0xff0000);
	putchar('\n', 0xff0000);

	// int num = customAtoi(argv[1]);
	while (1) {
		asm_sleep(10);

		take_fork(num);
		puts("Tome el tenedor ", 0xff0000);

		asm_sleep(10);

		put_fork(num);
		puts("deje el tenedor ", 0xff0000);
	}
}

void
philosophers_program()
{
	int i;
	mutex = asm_sem_open(GENERALSEMID, 1);
	for (i = 0; i < N; i++) {
		chopsticks[i] = asm_sem_open(i, 1);
	}

	char** args;
	char num[3];
	process_initialization p;
	p.file_descriptors = NULL;
	p.priority = 4;
	p.unkillable = 0;
	p.name = "My phylo";
	p.args = NULL;

	strcpy(args[0], "philo");
	for (i = 0; i < N; i++) {
		uint_to_base(i, num, 10);
		p.code = philosopher(i);

		pids[i] = asm_init_process(&p);
	}
	puts("Hola soy el filosofo y cree los procesos hijo\n", 0xff0000);

	char c;
	while (1) {
		asm_getchar(&c);
		switch (c) {
			case 'a':
			case 'A':
				if (n == MAX_PHILOSOPHERS) {
					puts("The table is full, no new philosophers can join\n", 0x0000ff);
				} else {
					uint_to_base(n, num, 10);
					strcpy(args[1], num);  // falta ver si tengo una func asi
					asm_sem_wait(mutex);
					chopsticks[n] = asm_sem_open(n, i);
					state[n] = THINKING;
					process_initialization p;
					p.code = philosopher(n);
					p.file_descriptors = NULL;
					p.priority = 4;
					p.unkillable = 0;
					p.name = "My phylo";
					p.args = NULL;
					pids[n++] = asm_init_process(&p);
					asm_sem_post(mutex);
					puts("The philosopher sat on the table\n", 0x0000ff);
				}
				break;
			case 'r':
			case 'R': {
				if (n <= 2) {
					puts("No more philosophers can leave\n", 0x0000ff);
				} else {
					asm_sem_wait(mutex);
					asm_kill_process(pids[n - 1], 0);
					n--;
					puts("The philosopher left the rift\n", 0x0000ff);
					asm_sem_post(mutex);
				}
			} break;
			case 'q':
			case 'Q': {
				asm_unblock_process(1);
			}break;
			default:
				break;
		}
	}
	exit();
}

void
print_table()
{
	int i;
	for (i = 0; i < n; i++) {
		if (state[i] == EATING) {
			puts("E ", 0xff0000);
		} else {
			puts(". ", 0x00ff00);
		}
	}
	putchar('\n', 0x00ff00);
}