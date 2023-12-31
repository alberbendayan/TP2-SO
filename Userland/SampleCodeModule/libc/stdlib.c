// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <stdlib.h>
#include <syscalls.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define DEV_NULL -1

#define READ 0
#define WRITE 1
#define ERROR 2

#define EOF -1

#define BUILT_IN_DESCRIPTORS 3

uint32_t
gets(char* buff, uint32_t size, uint32_t color)
{
	uint8_t state;
	char c;
	uint32_t len = 0;
	int* fd;
	fd = asm_get_fds();

	if (fd[READ] == STDIN) {
		while (!(((c = getchar(&state)) == '\n' || c== EOF )&& state == PRESSED)) {
			if (c && state == PRESSED) {
				if (c != '\b') {
					if (len < size - 1) {
						putchar(c, color);
						buff[len++] = c;
					}
				} else if (len > 0 && c != '\n') {
					if (buff[len - 1] == '\t')
						for (int i = 0; i < 7; i++)
							putchar(c, color);
					putchar(c, color);
					len--;
				}
			}
		}
		putchar('\n', color);
		if (c == EOF) {
			buff[len] = 0;
			buff[len+1]=EOF;
		} else {
			buff[len] = 0;
		}
		asm_free(fd);
		return len;
	} else if (fd[READ] == DEV_NULL) {
		asm_free(fd);
		buff[0] = EOF;
		return 0;
	} else if (fd[READ] < DEV_NULL) {
		asm_free(fd);
		return -1;
	} else if (fd[READ] >= BUILT_IN_DESCRIPTORS) {
		int lens = asm_read_pipe(fd[READ], buff, size);
		puts(buff, color);
		asm_free(fd);
		return lens;
	}
	return -1;
}

int16_t
getchar(uint8_t* state)
{
	int* fd;
	fd = asm_get_fds();
	char buff[2];
	if (fd[READ] == STDIN) {
		char c = asm_getchar(state);

		asm_free(fd);
		return c;
	} else if (fd[READ] == DEV_NULL) {
		asm_free(fd);
		buff[0] = EOF;
		return 0;
	} else if (fd[READ] < DEV_NULL) {
		asm_free(fd);
		return -1;
	} else if (fd[READ] >= BUILT_IN_DESCRIPTORS) {
		asm_read_pipe(fd[READ], buff, 1);
		*state = PRESSED;
		asm_free(fd);
		return buff[0];
	}
	return -1;
}

void
puts(char* str, uint32_t color)
{
	uint64_t len = strlen(str);
	int* fd;
	fd = asm_get_fds();
	if (fd[WRITE] == STDOUT || fd[WRITE] == STDERR) {
		for (int i = 0; i < len; i++)
			putchar(str[i], color);
	} else if (fd[WRITE] >= BUILT_IN_DESCRIPTORS) {
		asm_write_pipe(asm_get_current_id(), fd[WRITE], str, len);
	}
	asm_free(fd);
}

void
putchar(char c, uint32_t color)
{
	int* fd;
	fd = asm_get_fds();
	if (fd[WRITE] == STDOUT || fd[WRITE] == STDERR) {
		asm_putchar(c, color);
	}
	if (fd[WRITE] >= BUILT_IN_DESCRIPTORS) {
		asm_write_pipe(asm_get_current_id(), fd[WRITE], &c, 1);
	}
	asm_free(fd);
}

uint64_t
strlen(char* buff)
{
	uint64_t len = 0;
	while (buff[len++] != 0) {}
	return len - 1;
}

uint32_t
strtok(char* buff, uint8_t token, char** args, uint32_t size)
{
	int args_len = 0;
	if (*buff != token && *buff != 0)
		args[args_len++] = buff;
	while (*buff != 0) {
		if (*buff == token) {
			*buff = 0;
			if (*(buff + 1) != token && *(buff + 1) != 0) {
				if (args_len > size)
					break;
				args[args_len++] = buff + 1;
			}
		}
		buff++;
	}
	return args_len;
}

uint32_t
strcmp(char* s1, char* s2)
{
	while (*s1 != 0 && *s2 != 0 && *s1 == *s2) {
		s1++;
		s2++;
	}
	return *s1 == 0 && *s2 == 0;
}

uint32_t
uint_to_base(uint64_t value, char* buff, uint32_t base)
{
	char* p = buff;
	char *p1, *p2;
	uint32_t digits = 0;

	// Calculate characters for each digit
	do {
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	} while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	// Reverse string in buffer.
	p1 = buff;
	p2 = p - 1;
	while (p1 < p2) {
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}

uint8_t
is_hex_color_code(char* code)
{
	uint32_t len = strlen(code);
	if ((len != 7 || code[0] != '#') && (len != 8 || code[0] != '0' || code[1] != 'x'))
		return 0;

	char c;
	for (int i = len == 7 ? 1 : 2; i < len; i++) {
		c = code[i];
		if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')))
			return 0;
	}
	return 1;
}

uint32_t
hex_to_uint(char* code)
{
	uint32_t ret = 0;

	if (code[0] == '0' && code[1] == 'x')
		code += 2;
	else if (code[0] == '#')
		code++;

	for (int i = 0; code[i] != '\0'; i++) {
		char c = code[i];
		uint32_t value = 0;

		if (c >= '0' && c <= '9')
			value = c - '0';
		else if (c >= 'A' && c <= 'F')
			value = 10 + (c - 'A');
		else if (c >= 'a' && c <= 'f')
			value = 10 + (c - 'a');
		else
			return 0;

		ret = (ret << 4) | value;
	}

	return ret;
}

void
intToArray(int num, char* result)
{
	if (num == 0) {
		result[0] = '0';
		result[1] = '\0';
		return;
	}

	int i = 0;
	int isNegative = 0;

	if (num < 0) {
		isNegative = 1;
		num = -num;
	}

	while (num > 0) {
		int digit = num % 10;
		result[i] = '0' + digit;
		num /= 10;
		i++;
	}

	if (isNegative) {
		result[i] = '-';
		i++;
	}

	result[i] = '\0';

	// Invertir el resultado
	int start = 0;
	int end = i - 1;

	while (start < end) {
		char temp = result[start];
		result[start] = result[end];
		result[end] = temp;
		start++;
		end--;
	}
}

int
custom_atoi(char* str)
{
	int result = 0;
	int sign = 1;  // Para manejar el signo positivo o negativo

	// Manejar signos positivos o negativos
	if (*str == '-') {
		sign = -1;
		str++;  // Avanzar al siguiente carácter
	} else if (*str == '+') {
		str++;  // Avanzar al siguiente carácter
	}

	// Recorrer la cadena y construir el número entero
	while (*str >= '0' && *str <= '9') {
		result = result * 10 + (*str - '0');
		str++;
	}

	return result * sign;
}
uint8_t
is_vocal(char buf)
{
	return ((buf == 'a' || buf == 'e' || buf == 'i' || buf == 'o' || buf == 'u') ||
	        (buf == 'A' || buf == 'E' || buf == 'I' || buf == 'O' || buf == 'U'));
}

void*
strcpy(void* destination, const void* source, uint64_t length)
{
	/*
	 * memcpy does not support overlapping buffers, so always do it
	 * forwards. (Don't change this without adjusting memmove.)
	 *
	 * For speedy copying, optimize the common case where both pointers
	 * and the length are word-aligned, and copy word-at-a-time instead
	 * of byte-at-a-time. Otherwise, copy by bytes.
	 *
	 * The alignment logic below should be portable. We rely on
	 * the compiler to be reasonably intelligent about optimizing
	 * the divides and modulos out. Fortunately, it is.
	 */
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 && (uint64_t)source % sizeof(uint32_t) == 0 &&
	    length % sizeof(uint32_t) == 0) {
		uint32_t* d = (uint32_t*)destination;
		const uint32_t* s = (const uint32_t*)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	} else {
		uint8_t* d = (uint8_t*)destination;
		const uint8_t* s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}