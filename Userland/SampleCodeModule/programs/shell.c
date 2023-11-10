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

#define PONG_FG 0xf5ebbc
#define PONG_BG 0x151f42

#define DEV_NULL -1

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define NULL (void*)0
typedef struct
{
	uint32_t (*fn)();
	char *name, *desc;
} Command;

enum pipe_flag
{
	NO_PIPE = -1,
	PIPED_COMMAND_LEFT = 0,
	PIPED_COMMAND_RIGHT = 1
};

static Command commands[MAX_COMMANDS];
static char input_buffer[INPUT_SIZE];
static uint32_t commands_len = 0;
static uint8_t running = 1;

static void load_commands();
static void load_command(uint32_t (*fn)(), char* name, char* desc);
static int32_t process_input(char* buff, uint32_t size);
static int32_t process_commands(char* args[MAX_ARGS],
                                uint32_t args_len,
                                uint8_t foreground,
                                int fd[3],
                                enum pipe_flag pipe_flag);
static void prompt(int32_t status);

static uint32_t create_process(char** args, int* fd, char* name, int unkillable, int priority, void* code);

// commands
static uint32_t help(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag);
static uint32_t datetime(char* args[MAX_ARGS],
                         uint32_t args_len,
                         uint8_t foreground,
                         int fd[3],
                         enum pipe_flag pipe_flag);
static uint32_t exit();
static uint32_t printreg(char* args[MAX_ARGS],
                         uint32_t args_len,
                         uint8_t foreground,
                         int fd[3],
                         enum pipe_flag pipe_flag);
static uint32_t clear();
static uint32_t testioe();
static uint32_t testzde();
static uint32_t pong();
static uint32_t setcolor(char* args[MAX_ARGS],
                         uint32_t args_len,
                         uint8_t foreground,
                         int fd[3],
                         enum pipe_flag pipe_flag);
static uint32_t switchcolors();
static uint32_t memstatus(char* args[MAX_ARGS],
                          uint32_t args_len,
                          uint8_t foreground,
                          int fd[3],
                          enum pipe_flag pipe_flag);
static uint32_t ps(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag);
static uint32_t pid(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag);
static uint32_t kill(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag);
static uint32_t block(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag);
static uint32_t unblock(char* args[MAX_ARGS],
                        uint32_t args_len,
                        uint8_t foreground,
                        int fd[3],
                        enum pipe_flag pipe_flag);
static uint32_t nice(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag);
static uint32_t loop(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag);
static uint32_t yield();
static uint32_t phylos(char* args[MAX_ARGS],
                       uint32_t args_len,
                       uint8_t foreground,
                       int fd[3],
                       enum pipe_flag pipe_flag);
static uint32_t testmm(char* args[MAX_ARGS],
                       uint32_t args_len,
                       uint8_t foreground,
                       int fd[3],
                       enum pipe_flag pipe_flag);
static uint32_t testprio(char* args[MAX_ARGS],
                         uint32_t args_len,
                         uint8_t foreground,
                         int fd[3],
                         enum pipe_flag pipe_flag);
static uint32_t testproc(char* args[MAX_ARGS],
                         uint32_t args_len,
                         uint8_t foreground,
                         int fd[3],
                         enum pipe_flag pipe_flag);
static uint32_t testsync(char* args[MAX_ARGS],
                         uint32_t args_len,
                         uint8_t foreground,
                         int fd[3],
                         enum pipe_flag pipe_flag);

static int
array_len(char** array)
{
	int len = 0;
	while (*(array++) != NULL)
		len++;
	return len;
}

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
	load_command((main_function)yield, "yield", "            Renounce CPU");
	load_command((main_function)phylos, "phylos", "           Phylos");
	load_command((main_function)testmm, "testmm", "           test Memory Manager");
	load_command((main_function)testsync, "testsync", "         test syncro");
	load_command((main_function)testprio, "testprio", "         test pritority");
	load_command((main_function)testproc, "testproc", "         test process");
	
}

static void
load_command(uint32_t (*fn)(), char* name, char* desc)
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
process_input(char* buff, uint32_t size)
{
	char* args[MAX_ARGS];
	uint8_t foreground;
	uint32_t args_len = 0;
	args_len = strtok(buff, ' ', args, MAX_ARGS);
	args[args_len] = NULL;

	if (args_len == 0) {
		return -1;
	}

	if (strcmp(args[args_len - 1], "&")) {
		foreground = 0;
		args[args_len - 1] = NULL;
	} else {
		foreground = 1;
	}

	int pipe_pos = NO_PIPE;
	for (int i = 0; pipe_pos == NO_PIPE && i < args_len; i++) {
		if (strcmp(args[i], "|")) {
			pipe_pos = i;
		}
	}

	if (pipe_pos == NO_PIPE) {
		int fd_no_pipes[3] = { foreground ? STDIN : DEV_NULL, STDOUT, STDERR };
		return process_commands(args, args_len, foreground, fd_no_pipes, NO_PIPE);
	} else {
		puts("MAL", 0xffff00);
		return -1;
	}
}

static int32_t
process_commands(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	for (int i = 0; i < commands_len; i++) {
		if (strcmp(args[0], commands[i].name)) {
			return commands[i].fn(args, args_len, foreground, fd, pipe_flag);
		}
	}

	puts("Command not found: ", color.output);
	puts(args[0], color.output);
	putchar('\n', color.output);
	return -1;
}

static void
prompt(int32_t status)
{
	asm_sleep(6);
	puts(">>>", color.prompt);
	putchar(' ', color.fg);
	asm_block_process(1);
}

static uint32_t
create_process(char** args, int* fd, char* name, int unkillable, int priority, void* code)
{
	process_initialization p;

	p.args = args;
	p.file_descriptors = fd;
	p.name = name;
	p.unkillable = unkillable;
	p.priority = priority;
	p.code = code;

	int ret = asm_init_process(&p);
	asm_block_process(1); // 1 es el id de la shell

	return ret;
}

static uint32_t
func_help(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	if (foreground) {
		asm_block_process(1);
	}
	for (int i = 0; i < commands_len; i++) {
		puts(commands[i].name, color.output);
		puts(commands[i].desc, color.output);
		putchar('\n', color.output);
	}

	asm_unblock_process(1);
	return 0;
}

static uint32_t
help(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	create_process(args, fd, "help", 0, 4, &func_help);
}

static uint32_t
func_datetime()
{
	asm_block_process(1);
	asm_datetime(color.output);
	asm_unblock_process(1);
	return 0;
}

static uint32_t
datetime(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	create_process(args, fd, "datetime", 0, 4, &func_datetime);
}

// el clear lo dejo como builtin de la shell
static uint32_t
clear()
{
	asm_clear(color.bg);
	return 0;
}
// finaliza proceso
static uint32_t
exit()
{
	puts("Shell has finished", color.output);
	asm_kill_process(1, 0);
}

static uint32_t
func_printreg()
{
	asm_block_process(1);
	asm_printreg(color.output);
	asm_unblock_process(1);
	return 0;
}

static uint32_t
printreg(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	int pid = create_process(args, fd, "printreg", 0, 4, &func_printreg);
	return 0;
}

// las excepciones las dejamos asi xq no son requisito del tp
static uint32_t
testioe()
{
	asm_testioe();
	return 0;
}

static uint32_t
testzde()
{
	asm_testzde();
	return 0;
}
// builtin de la shell
static uint32_t
setcolor(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
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
	uint32_t targets_len = sizeof(targets) / sizeof(targets[0]);

	for (int i = 0; i < targets_len; i++) {
		if (strcmp(targets[i], args[1])) {
			uint32_t col = hex_to_uint(args[2]);
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
static uint32_t
switchcolors()
{
	uint32_t aux = color.bg;
	color.bg = color.fg;
	color.fg = aux;
	clear();
	return 0;
}

static uint32_t
func_memstatus()
{
	asm_block_process(1);
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
	asm_unblock_process(1);
	return 0;
}
static uint32_t
memstatus(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	int pid = create_process(args, fd, "memstatus", 0, 4, &func_memstatus);
	// return asm_kill_process(pid,0);
	return 0;
}

static uint32_t
func_ps()
{
	asm_block_process(1);
	char* string = asm_get_snapshots_info();
	puts(string, color.output);
	asm_free(string);
	asm_unblock_process(1);
	return 0;
}

static uint32_t
ps(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	int pid = create_process(args, fd, "ps", 0, 4, &func_ps);
	// return asm_kill_process(pid,0);
	return 0;
}

static uint32_t
func_pid()
{
	asm_block_process(1);
	char aux[6];
	uint64_t pid = asm_get_current_id();
	uint_to_base(pid, aux, 10);
	puts(aux, color.output);
	puts("\n", color.bg);
	asm_unblock_process(1);
	return 0;
}

static uint32_t
pid(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	create_process(args, fd, "pid", 0, 4, &func_pid);
}

static uint32_t
func_kill(int args_len, char* args[MAX_ARGS])
{
	asm_block_process(1);
	if (args_len == 1) {
		asm_kill_current_process(0);
	} else if (args_len == 2) {
		int arg = customAtoi(args[1]);

		asm_kill_process(arg, 0);
	} else if (args_len == 3) {
		asm_kill_process(customAtoi(args[1]), customAtoi(args[2]));  // los paso a int
	} else {
		char* usage = "USAGE: kill <pid> <ret value> or kill <pid>\n When leaving empty <pid> and <ret value> the "
		              "current process will be killed\n When leaving empty <ret value> the return value will be 0\n";
		puts(usage, color.output);
		asm_unblock_process(1);
		return 1;
	}
	asm_unblock_process(1);
	return 0;
}

static uint32_t
kill(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	create_process(args, fd, "kill", 0, 4, &func_kill);
}

static uint32_t
func_block(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	asm_block_process(1);
	if (args_len == 2) {
		int arg = customAtoi(args[1]);
		asm_block_process(arg);
		asm_unblock_process(1);
		return asm_kill_current_process(0);
	}
	char* usage = "USAGE: block <pid> \n";
	puts(usage, color.output);
	asm_unblock_process(1);
	return asm_kill_current_process(1);
}

static uint32_t
block(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	create_process(args, fd, "block", 0, 4, &func_block);
}

static void
func_unblock(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	asm_block_process(1);
	if (args_len == 2) {
		int arg = customAtoi(args[1]);
		asm_unblock_process(arg);
		asm_unblock_process(1);
		return asm_kill_current_process(0);
	}
	char* usage = "USAGE: unblock <pid> \n";
	puts(usage, color.output);
	asm_unblock_process(1);
	return asm_kill_current_process(1);
}

static uint32_t
unblock(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	create_process(args, fd, "unblock", 0, 4, &func_unblock);
}

void
func_nice(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	asm_block_process(1);
	if (args_len == 3) {
		int pid = customAtoi(args[1]);
		int new_priority = customAtoi(args[2]);
		asm_set_priority(pid, new_priority);
		asm_unblock_process(1);
		return 0;
	}
	char* usage = "USAGE: nice <pid> <new status> \n";
	puts(usage, color.output);
	asm_unblock_process(1);
	return 1;
}

static uint32_t
nice(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	int pid = create_process(args, fd, "nice", 0, 4, &func_nice);
	return asm_kill_process(pid, 0);
}

static uint32_t
yield()
{
	asm_yield();
}

void
func_loop(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	if (foreground) {
		asm_block_process(1);
	}
	char aux[6];
	uint64_t pid = asm_get_current_id();
	uint_to_base(pid, aux, 10);
	while (1) {
		puts("Hi! I'm process ", color.output);
		puts(aux, color.output);
		puts("\n", color.bg);
		asm_sleep(5 * 18);
	}
	asm_unblock_process(1);
}

static uint32_t
loop(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	return create_process(args, fd, "loop", 0, 4, &func_loop);
}

static uint32_t
phylos(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	return create_process(args, fd, "phylos", 0, 4, &run_philos);
}

static uint32_t
testmm(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	puts("Testeando el memory\n",0xff0000);
	return create_process(args, fd, "testmm", 0, 4, &test_mm);
}
static uint32_t
testprio(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	return create_process(args, fd, "testprio", 0, 4, &test_prio);
}
static uint32_t
testproc(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	return create_process(args, fd, "testproc", 0, 4, &test_processes);
}
static uint32_t
testsync(char* args[MAX_ARGS], uint32_t args_len, uint8_t foreground, int fd[3], enum pipe_flag pipe_flag)
{
	puts("Testeando sincronizacion\n",0xff0000);
	return create_process(args, fd, "test_sync", 0, 4, &test_sync);
}
