#include <libasm.h>
#include <phylos.h>
#include <shell.h>
#include <stdlib.h>
#include <syscalls.h>

#define MAX_QTY 8
#define MIN_QTY 3
#define MUTEX_SEM_ID 42
#define MAX_PHILO_NUMBER_BUFFER 3

#define EAT_TIME 3
#define THINK_TIME 2

#define COMMAND_QUIT 'q'
#define COMMAND_ADD 'a'
#define COMMAND_REMOVE 'r'
#define COMMAND_CLEAR 'c'

#define NULL (void*)0

#define left(i) (((i) + qty_philosophers - 1) % qty_philosophers)
#define right(i) (((i) + 1) % qty_philosophers)
#define philosopher_semaphore(i) (MUTEX_SEM_ID + (i) + 1)

typedef enum
{
	NONE = 0,
	EATING,
	HUNGRY,
	THINKING
} PHILOSOPHER_STATE;

static uint8_t qty_philosophers = 0;
static PHILOSOPHER_STATE philosopher_states[MAX_QTY];
static int16_t philosopher_pids[MAX_QTY];
static uint8_t single_line = 0;

static void render();
static int philosopher(int argc, char** argv);
static int8_t add_philosopher(int index);
static int8_t remove_philosopher(int index);
static void take_forks(int i);
static void put_forks(int i);
static void test(int i);

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

static const char* philosopher_names[] = {
	"Wancho",  "Coccaro", "Mazzanti", "Pusseto", "Tobio", "Alfonso", "Fattori",  "Echeverria",
	"Alarcon", "Fertoli", "Pereyra",  "Carrizo", "Souto", "Toranzo", "Espinoza",
};

int
run_philosophers(int argc, char** argv)
{
	puts("Bienvenido al programa de los filosofos version futbolera\n Es como el problema de los filosofos con la"
	     "leve modificacion que hay jugadores en ronda y hay un botin entre cada jugador\n un jugador para hacer el "
	     "gol necesita los dos botines\n - Con el a se agrega un jugador\n - Con la r (de 'Roja directa') echamos un "
	     "jugador\n - Con q terminamos el juego\n",
	     0xf0f0f0);
	qty_philosophers = 0;
	single_line = 0;
	if (asm_sem_open(MUTEX_SEM_ID, 1) == -1)
		return -1;

	for (int i = 0; i < MAX_QTY; i++) {
		philosopher_states[i] = NONE;
		philosopher_pids[i] = -1;
	}

	for (int i = 0; i < MIN_QTY + 2; i++)
		add_philosopher(i);

	char command = '\0';
	int aux;
	while ((command = asm_getchar(&aux)) != COMMAND_QUIT) {
		//asm_sleep(1);
		if (aux == 0) {
			continue;
		}
		switch (command) {
			case COMMAND_ADD:
				if (qty_philosophers < MAX_QTY) {
					if (add_philosopher(qty_philosophers) == -1)
						puts("No se pudo agregar un jugador\n", 0xf0f0f0);
				} else
					puts("La cancha esta llena\n", 0xf0f0f0);
				break;
			case COMMAND_REMOVE:
				if (qty_philosophers > MIN_QTY)
					remove_philosopher(qty_philosophers - 1);
				else
					puts("Como minimo debe haber 3 jugadores para empezar a patear\n", 0xf0f0f0);
				break;
			case COMMAND_CLEAR:
				single_line = !single_line;
				break;
		}
	}
	asm_sem_close(MUTEX_SEM_ID);
	asm_kill_current_process(0);
	return 0;
}

static void
render()
{
	const static char letters[] = { ' ', 'E', '.', '.' };
	uint8_t something_to_write = 0;
	for (int i = 0; i < qty_philosophers; i++) {
		if (letters[philosopher_states[i]] != ' ') {
			something_to_write = 1;
			if (philosopher_states[i] == EATING) {
				puts("G ", 0xff0000);
			} else {
				puts(". ", 0xff0000);
			}
		}
	}
	if (something_to_write)
		putchar('\n', 0xfffff);
	// sleep(1);
}

static int8_t
add_philosopher(int index)
{
	asm_sem_wait(MUTEX_SEM_ID);
	char philo_number_buffer[MAX_PHILO_NUMBER_BUFFER] = { 0 };
	if (asm_sem_open(philosopher_semaphore(index), 0) == -1) {
		return -1;
	}
	my_int_to_array(index, philo_number_buffer, 10);
	char* params[] = { "philosopher", philo_number_buffer, NULL };
	int file_descriptors[] = { -1, 1, 2 };

	process_initialization p;
	p.code = philosopher;

	p.name = params[0];
	p.args = params;
	p.file_descriptors = file_descriptors;
	p.priority = 4;
	p.unkillable = 0;
	

	philosopher_pids[index] = asm_init_process(&p);
	if (philosopher_pids[index] != -1) {
		qty_philosophers++;
	}
	render();
	asm_sem_post(MUTEX_SEM_ID);
	return -1 * !(philosopher_pids[index] + 1);
}

static int8_t
remove_philosopher(int index)
{
	puts("Roja directa para: ", 0xff0000);
	puts(philosopher_names[index], 0xff00ff);
	puts("\n", 0xffffff);

	asm_sem_wait(MUTEX_SEM_ID);

	while (philosopher_states[left(index)] == EATING && philosopher_states[right(index)] == EATING) {
		asm_sem_post(MUTEX_SEM_ID);
		asm_sem_wait(philosopher_semaphore(index));
		asm_sem_wait(MUTEX_SEM_ID);
	}

	asm_kill_process(philosopher_pids[index], 0);
	
	//asm_wait_pid(philosopher_pids[index]);
	asm_sem_close(philosopher_semaphore(index));
	philosopher_pids[index] = -1;
	philosopher_states[index] = NONE;
	qty_philosophers--;
	asm_sem_post(MUTEX_SEM_ID);
	render();
	return 0;
}

static int
philosopher(int argc, char** argv)
{
	int i = customAtoi(argv[1]);
	puts("Entre en el primer equipo del club atletico huracan: ", 0xff0000);
	puts(philosopher_names[i], 0xff00ff);
	puts("\n", 0xffffff);
	philosopher_states[i] = THINKING;
	while (1) {
		asm_sleep(5 * THINK_TIME);
		take_forks(i);
		asm_sleep(5 * EAT_TIME);
		put_forks(i);
	}
	return 0;
}

static void
take_forks(int i)
{
	asm_sem_wait(MUTEX_SEM_ID);
	philosopher_states[i] = HUNGRY;
	test(i);
	asm_sem_post(MUTEX_SEM_ID);
	asm_sem_wait(philosopher_semaphore(i));
}

static void
put_forks(int i)
{
	asm_sem_wait(MUTEX_SEM_ID);
	philosopher_states[i] = THINKING;
	render();
	test(left(i));
	test(right(i));
	asm_sem_post(MUTEX_SEM_ID);
}

static void
test(int i)
{
	if (philosopher_states[i] == HUNGRY && philosopher_states[left(i)] != EATING &&
	    philosopher_states[right(i)] != EATING) {
		philosopher_states[i] = EATING;
		render();
		asm_sem_post(philosopher_semaphore(i));
	}
}
