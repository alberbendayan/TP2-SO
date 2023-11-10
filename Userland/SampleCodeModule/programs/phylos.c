#include <libasm.h>
#include <phylos.h>
#include <shell.h>
#include <stdlib.h>
#include <syscalls.h>

// typedef int (*main_function)(int argc, char** args);

// typedef struct process_initialization
// {
// 	main_function code;
// 	char** args;
// 	char* name;
// 	uint8_t priority;
// 	int16_t* file_descriptors;
// 	uint8_t unkillable;
// } process_initialization;

#define NULL (void*)0

#define MAX 8
#define MIN 4
#define MUTEX_ID 999
#define SEM_ID 1000

phylosopher* phylos[MAX];
static int current_phylos = 0;
static int table_mutex;
static int working;

#define RIGHT(i) ((i) + 1) % (current_phylos)
#define LEFT(i) ((i) + current_phylos - 1) % (current_phylos)

void run_phylo(int argc, char* argv[]);
int add_phylo();
int remove_phylo();
void attempt_for_forks(int i);
void release_forks(int i);
void check_for_forks(int i);
void printer_assistant();
void lifecycle(int argc, char* argv[]);

void
reverse(char str[], int length)
{
	int start = 0;
	int end = length - 1;
	while (start < end) {
		char temp = str[start];
		str[start] = str[end];
		str[end] = temp;
		start++;
		end--;
	}
}

void
my_int_to_array(int num, char result[], int buffer_size)
{
	if (num == 0) {
		result[0] = '0';
		result[1] = '\0';
		return;
	}

	int is_negative = 0;

	if (num < 0) {
		is_negative = 1;
		num = -num;
	}

	int i = 0;
	while (num != 0) {
		int rem = num % 10;
		result[i++] = rem + '0';
		num = num / 10;
	}

	if (is_negative) {
		result[i++] = '-';
	}

	result[i] = '\0';

	reverse(result, i);
}

void
run_phylos(int argc, char* argv[])
{
	working = 1;
	table_mutex = asm_sem_open(MUTEX_ID, 1);
	if (table_mutex == -1) {
		puts("\nError opening semaphores! Returning...\n", 0xff0000);
		return;
	}
	puts("Problema de los filosofos.\n", 0x00ff00);
	puts("Instrucciones:\n- 'a': Agrega un filosofo\n- 'r': Borra un filosofo\n- 'q': Cierra el programa\nComencemos "
	     ":)\n\n",
	     0x00ff00);
	asm_sleep(18);

	for (int i = 0; i < MIN; i++) {
		add_phylo();
	}

	char* args[] = { "printer_assistant" };
	process_initialization p;
	int fd[3] = { 1, 1, 2 };
	p.name = args[0];
	p.args = args;
	p.file_descriptors = fd;
	p.priority = 4;
	p.unkillable = 0;
	p.code = &printer_assistant;
	int printer_assistant_pid = asm_init_process(&p);

	while (working) {
		int var;
		char key = asm_getchar(&var);
		// puts("getchar: ", 0x00ff00);
		// asm_putchar(key, 0x00ff00);
		// puts(" \n", 0x00ff00);
		switch (key) {
			case 'A':
			case 'a': {
				puts("Apretaste la A para agregar un filosofo\n", 0x00ff00);
				if (add_phylo() == -1) {
					puts("No se puede agregar (maximo 8)\n", 0x00ff00);
				} else {
					puts("Agregando filosofo...\n", 0x00ff00);
				}
			} break;
			case 'R':
			case 'r': {
				puts("Apretaste la R para matar un filosofo\n", 0x00ff00);
				if (remove_phylo() == -1) {
					puts("No se puede remover (minimo 4)\n", 0x00ff00);
				} else {
					puts("Removiendo filosofo...\n", 0x00ff00);
				}
			} break;
			case 'q': {
				puts("\nFinalizando.vuelva prontos\n\n", 0x00ff00);
				working = 0;
			} break;
			default:
				break;
		}
	}

	for (int i = 0; i < current_phylos; i++) {
		asm_sem_close(phylos[i]->sem);
		asm_kill_process(phylos[i]->pid, 0);
		asm_free(phylos[i]);
	}
	asm_kill_process(printer_assistant_pid, 0);
	asm_sem_close(MUTEX_ID);
}

int
add_phylo()
{
	if (current_phylos == MAX) {
		return -1;
	}
	asm_sem_wait(table_mutex);
	phylosopher* aux_phylo = asm_malloc(sizeof(phylosopher));
	if (aux_phylo == NULL) {
		puts("malloc es null\n", 0xff0000);
		return -1;
	}
	char c[5];
	my_int_to_array(current_phylos, c, 5);
	aux_phylo->id_phylo = current_phylos;

	char* args[] = { c, NULL };

	process_initialization p;
	int fd[3] = { -1, -1, 2 };  // corro los pibes en back
	p.name = args[0];
	p.args = args;
	p.file_descriptors = fd;
	p.priority = 4;
	p.unkillable = 0;
	p.code = lifecycle;

	aux_phylo->pid = asm_init_process(&p);

	aux_phylo->philo_state = THINKING;
	aux_phylo->sem = asm_sem_open(SEM_ID + current_phylos, 1);

	phylos[current_phylos++] = aux_phylo;

	asm_sem_post(table_mutex);

	return 0;
}

int
remove_phylo()
{
	if (current_phylos == MIN) {
		return -1;
	}

	phylosopher* chosen_philo = phylos[current_phylos];
	current_phylos--;
	asm_sem_close(chosen_philo->sem);
	asm_kill_process(chosen_philo->pid, 0);
	asm_free(chosen_philo);
	asm_sem_post(table_mutex);

	return 0;
}

void
lifecycle(int argc, char* argv[])
{
	// puts("Soy el wancho nro ", 0xff0000);
	// puts(argv[0], 0xff0000);
	// puts("\n", 0xff0000);

	int idx = customAtoi(argv[0]);
	while (working) {
		attempt_for_forks(idx);

		puts("tengo el fork y Soy el : ", 0xf0f00f);
		puts(argv[0], 0xf0f00f);
		puts("\n", 0xf0f00f);

		asm_sleep(1);

		release_forks(idx);

		puts("deje el fork y Soy el : ", 0xf0f00f);
		puts(argv[0], 0xf0f00f);
		puts("\n", 0xf0f00f);

		asm_sleep(1);
	}
}

void
attempt_for_forks(int i)
{
	char c[10];
	uint_to_base(i, c, 10);
	puts("pedi el fork y soy el : ", 0xf0f00f);
	puts(c, 0xf0f00f);
	puts("\n", 0xf0f00f);

	asm_sem_wait(table_mutex);

	phylos[i]->philo_state = HUNGRY;
	check_for_forks(i);

	asm_sem_post(table_mutex);

	asm_sem_wait(phylos[i]->sem);
}

void
release_forks(int i)
{
	char c[10];
	uint_to_base(i, c, 10);
	puts("deje el fork y soy el : ", 0xf0f00f);
	puts(c, 0xf0f00f);
	puts("\n", 0xf0f00f);

	asm_sem_wait(table_mutex);
	phylos[i]->philo_state = THINKING;
	check_for_forks(LEFT(i));
	check_for_forks(RIGHT(i));
	asm_sem_post(table_mutex);
}

void
check_for_forks(int i)
{
	char c[10];
	uint_to_base(i, c, 10);
	puts("checkeo el fork y soy el : ", 0xf0f00f);
	puts(c, 0xf0f00f);
	puts("\n", 0xf0f00f);	
	if (phylos[i]->philo_state == HUNGRY && phylos[LEFT(i)]->philo_state != EATING &&
	    phylos[RIGHT(i)]->philo_state != EATING) {

		phylos[i]->philo_state = EATING;
		asm_sem_post(phylos[i]->sem);
	}
}

void
printer_assistant(int argc, char* argv[])
{
	while (working) {
		puts("Antes de pasar el mutex desde el printer \n", 0xff00f0);
		asm_sem_wait(table_mutex);
		puts("Pase el mutex desde el printer \n", 0xff00f0);

		for (int i = 0; i < current_phylos; i++) {
			if (phylos[i]->philo_state == EATING) {
				putchar('E', 0xff0000);
			} else {
				putchar('.', 0xff0000);
			}
			putchar(' ', 0xff0000);
		}
		putchar('\n', 0xff0000);

		asm_sem_post(table_mutex);

		asm_yield();
	}
}
