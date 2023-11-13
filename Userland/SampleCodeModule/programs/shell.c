// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libasm.h>
#include <phylos.h>
#include <pong.h>
#include <shell.h>
#include <stdlib.h>
#include <syscalls.h>
#include <tests.h>

#define MAX_COMMANDS 30
#define MAX_ARGS 8
#define INPUT_SIZE 200

#define BUFFER_SIZE 4096

#define PONG_FG 0xf5ebbc
#define PONG_BG 0x151f42

#define DEV_NULL -1

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define EOF -1
#define NO_PIPE -1

#define NULL (void*)0
typedef struct
{
	int (*fn)();
	char *name, *desc;
} Command;

enum fds_positions
{
	READ = 0,
	WRITE = 1,
	ERR = 2
};

static Command commands[MAX_COMMANDS];
static char input_buffer[INPUT_SIZE];
static int commands_len = 0;
static uint8_t running = 1;

static void load_commands();
static void load_command(int (*fn)(), char* name, char* desc);
static int32_t process_input(char* buff, int size);
static int32_t process_commands(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static void prompt(int32_t status);

static int
create_process(char** args, int* fd, char* name, int unkillable, int priority, void* code, uint8_t foreground);

// commands
static int help(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static int datetime(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static int exit();
static int printreg(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static int clear();
static int testioe();
static int testzde();
static int setcolor(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static int switchcolors();
static int memstatus(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static int ps(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static int pid(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static int kill(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static int block(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static int unblock(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static int nice(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static int loop(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static int cat(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static int filter(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static int wc(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static int yield();
static int phylos(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static int testmm(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static int testprio(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static int testproc(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);
static int testsync(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3]);

uint32_t
shell_init()
{
	puts("Welcome to the shell!\nStart by typing 'help' on the prompt\n", color.output);
	load_commands();
	running = 1;

	int32_t len, status = 0;
	while (running) {
		prompt(status);
		len = gets(input_buffer, INPUT_SIZE, color.fg);
		status = process_input(input_buffer, len);
	}

	return status;
}

static void
load_commands()
{
	load_command((main_function)help, "help", "             Displays this help message");
	load_command((main_function)datetime, "datetime", "         Prints the current datetime");
	load_command((main_function)printreg,
	             "printreg",
	             "         Prints all the registers values saved in the last key press of 'Ctrl+r'");
	load_command(
	    (main_function)setcolor, "setcolor", "         Sets foreground, background, prompt, output or error colors");
	load_command((main_function)switchcolors, "switchcolors", "     Inverts the background and foreground colors");
	load_command((main_function)clear, "clear", "            Clears the screen");
	load_command((main_function)testioe, "testioe", "          Tests the 'Invalid Opcode Exception'");
	load_command((main_function)testzde, "testzde", "          Tests the 'Zero Division Error Exception'");
	load_command((main_function)exit, "exit", "             Exits the shell");
	load_command((main_function)memstatus, "mem", "              Displays memory status");
	load_command((main_function)ps, "ps", "               Displays status of all processes");
	load_command((main_function)pid, "pid", "              Displays current process id");
	load_command((main_function)kill, "kill", "             Kill a process by id");
	load_command((main_function)block, "block", "            Block a process by id");
	load_command((main_function)unblock, "unblock", "          Unblock a process by id");
	load_command((main_function)nice, "nice", "             Change process priority by id");
	load_command((main_function)loop, "loop", "             Print current process id");
	load_command((main_function)cat, "cat", "              Print STDIN");
	load_command((main_function)filter, "filter", "           Filter vowels");
	load_command((main_function)wc, "wc", "               Print the number of lines");
	load_command((main_function)yield, "yield", "            Renounce CPU");
	load_command((main_function)phylos, "phylos", "           Phylos");
	load_command((main_function)testmm, "testmm", "           test Memory Manager");
	load_command((main_function)testsync, "testsync", "         test syncro");
	load_command((main_function)testprio, "testprio", "         test pritority");
	load_command((main_function)testproc, "testproc", "         test process");
}

static void
load_command(int (*fn)(), char* name, char* desc)
// load_command(, char* name, char* desc)
{
	if (commands_len >= MAX_COMMANDS)
		return;
	commands[commands_len].fn = fn;
	commands[commands_len].name = name;
	commands[commands_len].desc = desc;
	commands_len++;
}

static int32_t
process_input(char* buff, int size)
{
	char* args[MAX_ARGS];
	uint8_t foreground;
	int args_len = 0;
	args_len = strtok(buff, ' ', args, MAX_ARGS);
	args[args_len] = NULL;

	if (args_len == 0) {
		return -1;
	}

	if (strcmp(args[args_len - 1], "&")) {
		foreground = 0;
		args[--args_len] = NULL;
	} else {
		foreground = 1;
	}

	int pipe_pos = NO_PIPE;
	for (int i = 0; pipe_pos == NO_PIPE && i < args_len; i++) {
		if (strcmp(args[i], "|")) {
			pipe_pos = i;
			args[i] = NULL;
		}
	}

	if (pipe_pos == NO_PIPE) {
		int fd_no_pipes[3] = { foreground ? STDIN : DEV_NULL, STDOUT, STDERR };
		return process_commands(args, args_len, foreground, fd_no_pipes);
	} else {
		int pipe_id = asm_get_last_free_pipe();
		int fd_left[3] = { STDIN, pipe_id, STDERR };
		int fd_right[3] = { pipe_id, STDOUT, STDERR };

		int left = process_commands(args, pipe_pos, foreground, fd_left);
		asm_wait_pid(left);
		// process_commands(args+pipe_pos + 1, args_len - pipe_pos - 1, foreground, fd_right); // tira warnings
		process_commands(&args[pipe_pos + 1], args_len - pipe_pos - 1, foreground, fd_right);
		return -1;
	}
}

static int32_t
process_commands(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	for (int i = 0; i < commands_len; i++) {
		if (strcmp(args[0], commands[i].name)) {
			return commands[i].fn(args, args_len, foreground, fd);
		}
	}

	puts("Command not found: ", color.output);
	puts(args[0], color.output);
	puts("\n", color.output);
	return -1;
}

static void
prompt(int32_t status)
{
	asm_sleep(9);
	puts(">>>", color.prompt);
	puts(" ", color.fg);
	// asm_block_process(1);
}

static int
create_process(char** args, int* fd, char* name, int unkillable, int priority, void* code, uint8_t foreground)
{
	process_initialization p;
	p.args = args;
	p.file_descriptors = fd;
	p.name = name;
	p.unkillable = unkillable;
	p.priority = priority;
	p.code = code;

	int ret = asm_init_process(&p);

	if (foreground) {
		asm_wait_pid(ret);
	}

	return ret;
}

static int
func_help(int args_len, char* args[MAX_ARGS])
{
	for (int i = 0; i < commands_len; i++) {
		puts(commands[i].name, color.output);
		puts(commands[i].desc, color.output);
		putchar('\n', color.output);
	}
	return 0;
}

static int
help(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	return create_process(args, fd, "help", 0, 4, &func_help, foreground);
}

static int
func_datetime(int args_len, char* args[MAX_ARGS])
{
	asm_datetime(color.output);
	return 0;
}

static int
datetime(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	return create_process(args, fd, "datetime", 0, 4, &func_datetime, foreground);
}

// el clear lo dejo como builtin de la shell
static int
clear()
{
	asm_clear(color.bg);
	return 0;
}
// finaliza proceso
static int
exit()
{
	puts("Shell has finished", color.output);
	asm_kill_process(1, 0);
	return 0;
}

static int
func_printreg(int args_len, char* args[MAX_ARGS])
{
	asm_printreg(color.output);
	return 0;
}

static int
printreg(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	return create_process(args, fd, "printreg", 0, 4, &func_printreg, foreground);
}

// las excepciones las dejamos asi xq no son requisito del tp
static int
testioe()
{
	asm_testioe();
	return 0;
}

static int
testzde()
{
	asm_testzde();
	return 0;
}
// builtin de la shell
static int
setcolor(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	if (args_len != 3) {
		puts("USAGE: setcolor <target> <color>\n<target>   fg | bg | output | prompt | error\n<color>    The color in "
		     "hex format ('0xXXXXXX' or '#XXXXXX')\n",
		     color.output);
		return -1;
	}

	if (!is_hex_color_code(args[2])) {
		puts("Invalid color '", color.output);
		puts(args[2], color.output);
		puts("'\n", color.output);
		return -1;
	}

	char* targets[] = { "fg", "bg", "output", "prompt", "error" };
	int targets_len = sizeof(targets) / sizeof(targets[0]);

	for (int i = 0; i < targets_len; i++) {
		if (strcmp(targets[i], args[1])) {
			int col = hex_to_uint(args[2]);
			switch (i) {
				case 0: {
					color.fg = col;
				} break;

				case 1: {
					color.bg = col;
					clear();
				} break;

				case 2: {
					color.output = col;
				} break;

				case 3: {
					color.prompt = col;
				} break;

				case 4: {
					color.error = col;
				} break;
			}
			return 0;
		}
	}

	puts("Invalid target '", color.output);
	puts(args[1], color.output);
	puts("'\n", color.output);
	return -1;
}
// builtin de la shell
static int
switchcolors()
{
	int aux = color.bg;
	color.bg = color.fg;
	color.fg = aux;
	clear();
	return 0;
}

static int
func_memstatus(int args_len, char* args[MAX_ARGS])
{
	char c1[32], c2[32], c3[32];
	uint_to_base(asm_total_heap(), c1, 10);
	uint_to_base(asm_free_heap(), c2, 10);
	uint_to_base(asm_used_heap(), c3, 10);
	puts("Total heap: ", color.output);
	puts(c1, color.output);
	puts("\n", color.output);
	puts("Free heap: ", color.output);
	puts(c2, color.output);
	puts("\n", color.output);
	puts("Used heap: ", color.output);
	puts(c3, color.output);
	puts("\n", color.output);
	return 0;
}
static int
memstatus(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	return create_process(args, fd, "memstatus", 0, 4, &func_memstatus, foreground);
}

static int
func_ps(int args_len, char* args[MAX_ARGS])
{
	char* string = asm_get_snapshots_info();
	puts(string, color.output);
	asm_free(string);
	return 0;
}

static int
ps(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	return create_process(args, fd, "ps", 0, 4, &func_ps, foreground);
}

static int
func_pid(int args_len, char* args[MAX_ARGS])
{
	char aux[6];
	uint64_t pid = asm_get_current_id();
	uint_to_base(pid, aux, 10);
	puts(aux, color.output);
	puts("\n", color.bg);
	return 0;
}

static int
pid(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	return create_process(args, fd, "pid", 0, 4, &func_pid, foreground);
}

static int
func_kill(int args_len, char* args[MAX_ARGS])
{
	if (args_len == 2) {
		int arg = custom_atoi(args[1]);

		asm_kill_process(arg, 0);
	} else if (args_len == 3) {
		asm_kill_process(custom_atoi(args[1]), custom_atoi(args[2]));  // los paso a int
	} else {
		char* usage = "USAGE: kill <pid> <ret value> or kill <pid>\n When leaving empty <pid> and <ret value> the "
		              "current process will be killed\n When leaving empty <ret value> the return value will be 0\n";
		puts(usage, color.output);
		asm_unblock_process(1);
		return 1;
	}
	return 0;
}

static int
kill(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	return create_process(args, fd, "kill", 0, 4, &func_kill, foreground);
}

static int
func_block(int args_len, char* args[MAX_ARGS])
{
	if (args_len == 2) {
		int arg = custom_atoi(args[1]);
		asm_block_process(arg);
		asm_unblock_process(1);
		return asm_kill_current_process(0);
	}
	char* usage = "USAGE: block <pid> \n";
	puts(usage, color.output);
	return asm_kill_current_process(1);
}

static int
block(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	return create_process(args, fd, "block", 0, 4, &func_block, foreground);

	return 0;
}

static void
func_unblock(int args_len, char* args[MAX_ARGS])
{
	if (args_len == 2) {
		int arg = custom_atoi(args[1]);
		asm_unblock_process(arg);
		asm_unblock_process(1);
		asm_kill_current_process(0);
		return;
	}
	char* usage = "USAGE: unblock <pid> \n";
	puts(usage, color.output);
	asm_kill_current_process(1);
}

static int
unblock(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	return create_process(args, fd, "unblock", 0, 4, &func_unblock, foreground);

	return 0;
}

void
func_nice(int args_len, char* args[MAX_ARGS])
{
	if (args_len == 3) {
		int pid = custom_atoi(args[1]);
		int new_priority = custom_atoi(args[2]);
		asm_set_priority(pid, new_priority);
		asm_unblock_process(1);
		return;
	}
	char* usage = "USAGE: nice <pid> <new status> \n";
	puts(usage, color.output);
	asm_unblock_process(1);
	return;
}

static int
nice(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	int pid = create_process(args, fd, "nice", 0, 4, &func_nice, foreground);

	return asm_kill_process(pid, 0);
}

static int
yield()
{
	asm_yield();
	return 0;
}

void
func_loop(int args_len, char* args[MAX_ARGS])
{
	char aux[6];
	uint64_t pid = asm_get_current_id();
	uint_to_base(pid, aux, 10);
	while (1) {
		puts("Hi! I'm process ", color.output);
		puts(aux, color.output);
		puts("\n", color.bg);
		asm_sleep(5 * 18);
	}
	return;
}

static int
loop(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	return create_process(args, fd, "loop", 0, 4, &func_loop, foreground);
}

void
func_cat(int args_len, char* args[MAX_ARGS])
{
	char buffer[1024];
	int len;

	while (1) {
		len = gets(buffer, 1024, color.fg);

		if (buffer[len + 1] == EOF) {
			return;
		}

		puts(buffer, color.output);
		puts("\n", color.output);
	}
}
static int
cat(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	return create_process(args, fd, "cat", 0, 4, &func_cat, foreground);
}

void
func_filter(int args_len, char* args[MAX_ARGS])
{
	char buffer = 0;
	uint8_t state;

	while (1) {
		while (state != PRESSED) {
			buffer = getchar(&state);
		}

		if (buffer == EOF) {
			putchar('\n', color.output);
			return;
		}

		if (!is_vocal(buffer)) {
			putchar(buffer, color.output);
		}
		state = 0;
	}
}
static int
filter(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	return create_process(args, fd, "filter", 0, 4, &func_filter, foreground);
}

void
func_wc(char argc, char** argv)
{
	char buffer = 0;
	uint64_t count = 1;
	uint8_t state;

	while (!(buffer == EOF)) {
		while (state != PRESSED) {
			buffer = getchar(&state);
		}
		if (buffer == '\n') {
			count++;
		}
		state = 0;
		int *fd = asm_get_fds();
		if (fd[READ]==STDIN) {
			putchar(buffer, color.output);
		}
		asm_free(fd);
	}
	putchar('\n', color.output);
	puts("Cantidad de lineas: ", color.output);
	char aux[10];
	uint_to_base(count, aux, 10);
	puts(aux, color.output);
	putchar('\n', color.output);
	return;
}
static int
wc(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	return create_process(args, fd, "wc", 0, 4, &func_wc, foreground);
}

static int
phylos(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	int pid = create_process(args, fd, "phylos", 0, 4, &run_philosophers, foreground);
	asm_wait_pid(pid);
	return pid;
}

static int
testmm(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	puts("Testeando el memory\n", 0xff0000);
	return create_process(args, fd, "testmm", 0, 4, &test_mm, foreground);
}
static int
testprio(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	return create_process(args, fd, "testprio", 0, 4, &test_prio, foreground);
}
static int
testproc(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	return create_process(args, fd, "testproc", 0, 4, &test_processes, foreground);
}
static int
testsync(char* args[MAX_ARGS], int args_len, uint8_t foreground, int fd[3])
{
	puts("Testeando sincronizacion\n", 0xff0000);
	return create_process(args, fd, "test_sync", 0, 4, &test_sync, foreground);
}
