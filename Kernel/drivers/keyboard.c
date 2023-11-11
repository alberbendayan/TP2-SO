#include <font.h>
#include <keyboard.h>
#include <libasm.h>
#include <libc.h>
#include <naiveConsole.h>
#include <scheduler.h>
#include <stdint.h>
#include <text.h>
#include <video.h>

#define KC_L_SHIFT 42
#define KC_R_SHIFT 54
#define KC_L_SHIFT_RELEASE KC_L_SHIFT + 128
#define KC_R_SHIFT_REREASE KC_R_SHIFT + 128
#define KC_CAPS_LOCK 58
#define KC_L_CTRL 29
#define KC_L_CTRL_RELEASE KC_L_CTRL + 128

#define BUFFER_MAX 10
#define REGISTER_CAPTURE 9
#define KILL_FOREGROUND_PROCESS 99
#define RELEASED 0
#define PRESSED 1

// define los caracteres para cada código. al presionar shift se utiliza la segunda columna
static const uint8_t scancodes[][2] = {
	{ 0, 0 },       { 0, 0 },       { '1', '!' }, { '2', '@' }, { '3', '#' }, { '4', '$' },  { '5', '%' },
	{ '6', '^' },   { '7', '&' },   { '8', '*' }, { '9', '(' }, { '0', ')' }, { '-', '_' },  { '=', '+' },
	{ '\b', '\b' }, { '\t', '\t' }, { 'q', 'Q' }, { 'w', 'W' }, { 'e', 'E' }, { 'r', 'R' },  { 't', 'T' },
	{ 'y', 'Y' },   { 'u', 'U' },   { 'i', 'I' }, { 'o', 'O' }, { 'p', 'P' }, { '[', '{' },  { ']', '}' },
	{ '\n', '\n' }, { 0, 0 },       { 'a', 'A' }, { 's', 'S' }, { 'd', 'D' }, { 'f', 'F' },  { 'g', 'G' },
	{ 'h', 'H' },   { 'j', 'J' },   { 'k', 'K' }, { 'l', 'L' }, { ';', ':' }, { '\'', '"' }, { '`', '~' },
	{ 0, 0 },       { '\\', '|' },  { 'z', 'Z' }, { 'x', 'X' }, { 'c', 'C' }, { 'v', 'V' },  { 'b', 'B' },
	{ 'n', 'N' },   { 'm', 'M' },   { ',', '<' }, { '.', '>' }, { '/', '?' }, { 0, 0 },      { 0, 0 },
	{ 0, 0 },       { ' ', ' ' },
};
static const uint32_t keys = sizeof(scancodes) / sizeof(scancodes[0]);

// caracteres de control
static uint8_t shift = 0, caps_lock = 0, control = 0;

// buffers de almacenamiento para el caracter y el estado del mismo (PRESSED o RELEASED)
static uint8_t buffer_chars[BUFFER_MAX];
static uint8_t buffer_states[BUFFER_MAX];
static uint32_t buffer_size = 0;

static uint8_t get_scancode(uint8_t key);
static void put_buffer(uint8_t code, uint8_t state);
static void eof();

int waiting_processes[4096];
int i = 0;

int
keyboard_handler()
{
	int key;
	while (asm_kbd_active()) {
		key = asm_kbd_getkey();

		if (key == KC_L_SHIFT || key == KC_R_SHIFT)
			shift = 1;
		else if (key == KC_L_SHIFT_RELEASE || key == KC_R_SHIFT_REREASE)
			shift = 0;
		if (key == KC_CAPS_LOCK)
			caps_lock = !caps_lock;
		if (key == KC_L_CTRL)
			control = 1;
		else if (key == KC_L_CTRL_RELEASE)
			control = 0;

		uint8_t code, state;

		// aunque el caracter haya sido soltado, quiero guardar su ASCII
		state = (key & 0x80 ? RELEASED : PRESSED);
		key -= (key & 0x80 ? 0x80 : 0);
		code = get_scancode(key);

		// handle para casos especiales
		if (control && (code == 'r' || code == 'R')) {
			return REGISTER_CAPTURE;
		} else if (control && (code == 'c' || code == 'C')) {
			return kill_foreground_process();
		} else if (control && (code == 'd' || code == 'D')) {
			eof();
		} else if (key >= 0 && key < keys && code != 0) {
			put_buffer(code, state);
		}
	}
	return 0;
}

char
kb_getchar(uint8_t* state)
{
	if (buffer_size <= 0) {
		// if (i < 4096) {
		// 	// tx_put_word("Bloqueo\n",0xff0000);
		// 	waiting_processes[i++] = get_pid();
		// 	block_process(get_pid());
		// }
		return 0;
	}

	// agarramos el primero agregado (como una queue)
	uint8_t key = buffer_chars[0];
	*state = buffer_states[0];

	// movemos los valores restantes una posición adelante
	for (int i = 1; i < buffer_size; i++) {
		buffer_chars[i - 1] = buffer_chars[i];
		buffer_states[i - 1] = buffer_states[i];
	}

	buffer_size--;
	return key;
}

static uint8_t
get_scancode(uint8_t key)
{
	uint8_t c;
	if (caps_lock && !shift) {
		c = scancodes[key][0];
		if (c >= 'A' && c <= 'z')
			c = scancodes[key][1];
	} else {
		c = scancodes[key][shift];
	}
	return c;
}

static void
put_buffer(uint8_t code, uint8_t state)
{
	if (buffer_size < BUFFER_MAX) {
		buffer_chars[buffer_size] = code;
		buffer_states[buffer_size++] = state;
		// for (int h = 0; h < i; h++) {
		// 	unblock_process(waiting_processes[h]);
		// }
		// i = 0;
	}
}
#define EOF -1
static void
eof()
{
	put_buffer(EOF, PRESSED);
	put_buffer(EOF, RELEASED);
}