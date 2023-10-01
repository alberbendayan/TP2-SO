#include <libasm.h>
#include <pong.h>
#include <shell.h>
#include <stdlib.h>
#include <syscalls.h>

#define MAX_COMMANDS 20
#define MAX_ARGS 3
#define INPUT_SIZE 200

#define PONG_FG 0xf5ebbc
#define PONG_BG 0x151f42

typedef struct
{
	uint32_t (*fn)();
	char *name, *desc;
} Command;

typedef struct
{
	volatile uint32_t fg, bg, output, prompt, error;
} Color;

static Command commands[MAX_COMMANDS];
static char* args[MAX_ARGS];
static char input_buffer[INPUT_SIZE];
static uint32_t commands_len = 0;
static uint32_t args_len = 0;
static uint8_t running = 1;

// colors
static Color color = { .fg = 0xffffff, .bg = 0x000000, .output = 0xa0a0a0, .prompt = 0x00ff00, .error = 0xff0000 };

static void load_commands();
static void load_command(uint32_t (*fn)(), char* name, char* desc);
static int32_t process_input(char* buff, uint32_t size);
static void prompt(int32_t status);

// commands
static uint32_t help();
static uint32_t datetime();
static uint32_t exit();
static uint32_t printreg();
static uint32_t clear();
static uint32_t testioe();
static uint32_t testzde();
static uint32_t pong();
static uint32_t setcolor();
static uint32_t switchcolors();

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
	load_command(help, "help", "          Displays this help message");
	load_command(datetime, "datetime", "      Prints the current datetime");
	load_command(printreg, "printreg", "      Prints all the registers values saved in the last key press of 'Ctrl+r'");
	load_command(pong, "pong", "          Pong (The Game)");
	load_command(setcolor, "setcolor", "      Sets foreground, background, prompt, output or error colors");
	load_command(switchcolors, "switchcolors", "  Inverts the background and foreground colors");
	load_command(clear, "clear", "         Clears the screen");
	load_command(testioe, "testioe", "       Tests the 'Invalid Opcode Exception'");
	load_command(testzde, "testzde", "       Tests the 'Zero Division Error Exception'");
	load_command(exit, "exit", "          Exits the shell");
}

static void
load_command(uint32_t (*fn)(), char* name, char* desc)
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
	args_len = strtok(buff, ' ', args, MAX_ARGS);
	if (args_len == 0)
		return -1;

	for (int i = 0; i < commands_len; i++) {
		if (strcmp(args[0], commands[i].name))
			return commands[i].fn();
	}
	puts("Command not found: ", color.output);
	puts(args[0], color.output);
	putchar('\n', color.output);
	return -1;
}

static void
prompt(int32_t status)
{
	puts(">>>", status == 0 ? color.prompt : color.error);
	putchar(' ', color.fg);
}

static uint32_t
help()
{
	for (int i = 0; i < commands_len; i++) {
		puts(commands[i].name, color.output);
		puts(commands[i].desc, color.output);
		putchar('\n', color.output);
	}
	return 0;
}

static uint32_t
datetime()
{
	asm_datetime(color.output);
	return 0;
}

static uint32_t
clear()
{
	asm_clear(color.bg);
	return 0;
}

static uint32_t
exit()
{
	return running = 0;
}

static uint32_t
printreg()
{
	asm_printreg(color.output);
	return 0;
}

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

static uint32_t
pong()
{
	char* usage = "USAGE: pong [fg] [bg]\nWhen leaving empty, <fg> and <bg> will get default values\n";

	if (args_len == 1) {
		start_game(PONG_FG, PONG_BG);
	} else if (args_len == 3) {
		if (!is_hex_color_code(args[1]) || !is_hex_color_code(args[2])) {
			puts("Invalid arguments\n", color.output);
			puts(usage, color.output);
			return -1;
		}
		uint32_t fg = hex_to_uint(args[1]);
		uint32_t bg = hex_to_uint(args[2]);
		start_game(fg, bg);
	} else {
		puts("Invalid ammount of arguments\n", color.output);
		puts(usage, color.output);
		return -1;
	}
	asm_clear(color.bg);
	return 0;
}

static uint32_t
setcolor()
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

static uint32_t
switchcolors()
{
	uint32_t aux = color.bg;
	color.bg = color.fg;
	color.fg = aux;
	clear();
	return 0;
}
