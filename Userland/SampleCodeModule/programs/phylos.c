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

philosopher* philos[MAX];
static int current_philos = 0;
static int table_mutex;
static int working;

int phylo_id = 1;

#define RIGHT(i) ((i) + 1) % (current_philos)
#define LEFT(i) ((i) + current_philos - 1) % (current_philos)

void run_philo(int argc, char* argv[]);
int add_philo();
int remove_philo();
void attempt_for_forks(int i);
void release_forks(int i);
void check_for_forks(int i);
void printer_assistant();
void lifecycle(int argc, char* argv[]);

void reverse(char str[], int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        // Intercambiar los caracteres en las posiciones start y end
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void myIntToArray(int num, char result[], int bufferSize) {
    // Manejar el caso especial de 0
    if (num == 0) {
        result[0] = '0';
        result[1] = '\0';
        return;
    }

    int isNegative = 0;

    // Manejar números negativos
    if (num < 0) {
        isNegative = 1;
        num = -num;
    }

    int i = 0;
    while (num != 0) {
        int rem = num % 10;
        result[i++] = rem + '0'; // Convertir el dígito a carácter y almacenar en el resultado
        num = num / 10;
    }

    // Agregar el signo negativo si es necesario
    if (isNegative) {
        result[i++] = '-';
    }

    // Agregar el terminador nulo
    result[i] = '\0';

    // Invertir la cadena para obtener la representación correcta
    reverse(result, i);
}

void
run_philos(int argc, char* argv[])
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
		add_philo();
		puts("Agrego phylo\n", 0xf0f0f0);
	}
	char* args[] = { "printer_assistant" };
	process_initialization p;
	int fd[3] = { 0, 1, 2 };
	p.name = args[0];
	p.args = args;
	p.file_descriptors = fd;
	p.priority = 4;
	p.unkillable = 0;
	p.code = &printer_assistant;
	int printer_assistant_pid = asm_init_process(&p);
	while (working) {
		char key;
		asm_getchar(&key);
		switch (key) {
			case 'A':
			case 'a': {
				if (add_philo() == -1) {
					puts("No se puede agregar (maximo 8)\n", 0x00ff00);
				} else {
					puts("Agregando filosofo...\n", 0x00ff00);
				}
			} break;
			case 'R':
			case 'r': {
				if (remove_philo() == -1) {
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

	for (int i = 0; i < current_philos; i++) {
		asm_sem_close(philos[i]->sem);
		asm_kill_process(philos[i]->pid, 0);
		asm_free(philos[i]);
	}
	current_philos = 0;
	asm_kill_process(printer_assistant_pid, 0);
	asm_sem_close(MUTEX_ID);
}

int
add_philo()
{
	if (current_philos == MAX) {
		return -1;
	}
	asm_sem_wait(table_mutex);
	philosopher* aux_philo = asm_malloc(sizeof(philosopher));
	if (aux_philo == NULL) {
		return -1;
	}
	// process_initialization p;
	char c[5];
    myIntToArray(phylo_id,c,5);
	phylo_id++;

    char* args[] = { c, NULL };
	process_initialization p;
	int fd[3] = { 0, 1, 2 };
	p.name = args[0];
	p.args = args;
	p.file_descriptors = fd;
	p.priority = 4;
	p.unkillable = 0;
	p.code = &lifecycle;

	
	aux_philo->pid = asm_init_process(&p);


	aux_philo->philo_state = THINKING;
	aux_philo->sem = asm_sem_open(SEM_ID + current_philos, 1);

	philos[current_philos++] = aux_philo;

	asm_sem_post(table_mutex);

	return 0;
}

int
remove_philo()
{
	if (current_philos == MIN) {
		return -1;
	}

	current_philos--;
	philosopher* chosen_philo = philos[current_philos];
	asm_sem_close(chosen_philo->sem);
	asm_kill_process(chosen_philo->pid, 0);
	asm_free(chosen_philo);
	asm_sem_post(table_mutex);

	return 0;
}

void
lifecycle(int argc, char* argv[])
{
	// puts("lifecycle for: ", 0xf0f00f);
	// puts(argv[1], 0xf0f00f);
	// puts("\n", 0xf0f00f);
	int idx = customAtoi(argv[1]);
	while (working) {
		attempt_for_forks(idx);

		asm_sleep(5);

		release_forks(idx);

		asm_sleep(5);
	}
}

void
attempt_for_forks(int i)
{
	asm_sem_wait(table_mutex);

	philos[i]->philo_state = HUNGRY;
	check_for_forks(i);

	asm_sem_post(table_mutex);

	asm_sem_wait(philos[i]->sem);
}

void
release_forks(int i)
{
	asm_sem_wait(table_mutex);
	philos[i]->philo_state = THINKING;
	check_for_forks(LEFT(i));
	check_for_forks(RIGHT(i));
	asm_sem_post(table_mutex);
}

void
check_for_forks(int i)
{
	if (philos[i]->philo_state == HUNGRY && philos[LEFT(i)]->philo_state != EATING &&
	    philos[RIGHT(i)]->philo_state != EATING) {
		philos[i]->philo_state = EATING;
		asm_sem_post(philos[i]->sem);
	}
}

void
printer_assistant(int argc, char* argv[])
{
	while (working) {
        // puts("Imprimiendo... cant de philos ",0xff00f0);
		// asm_putchar(current_philos+'0',0xff00f0);
			
        asm_sem_wait(table_mutex);
        
		for (int i = 0; i < current_philos; i++) {
            if (philos[i]->philo_state == EATING) {
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
