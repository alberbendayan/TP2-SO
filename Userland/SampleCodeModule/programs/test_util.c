// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "my_syscall.h"
#include <stdlib.h>
#include <stdint.h>


// Random
static uint32_t m_z = 362436069;
static uint32_t m_w = 521288629;

uint32_t
GetUint()
{
	m_z = 36969 * (m_z & 65535) + (m_z >> 16);
	m_w = 18000 * (m_w & 65535) + (m_w >> 16);
	return (m_z << 16) + m_w;
}

uint32_t
GetUniform(uint32_t max)
{
	uint32_t u = GetUint();
	return (u + 1.0) * 2.328306435454494e-10 * max;
}

// Memory
uint8_t
memcheck(void* start, uint8_t value, uint32_t size)
{
	uint8_t* p = (uint8_t*)start;
	uint32_t i;

	for (i = 0; i < size; i++, p++)
		if (*p != value)
			return 0;

	return 1;
}

// Parameters
int64_t
satoi(char* str)
{
	uint64_t i = 0;
	int64_t res = 0;
	int8_t sign = 1;

	if (!str)
		return 0;

	if (str[i] == '-') {
		i++;
		sign = -1;
	}

	for (; str[i] != '\0'; ++i) {
		if (str[i] < '0' || str[i] > '9')
			return 0;
		res = res * 10 + str[i] - '0';
	}

	return res * sign;
}

// Dummies
void
bussy_wait(uint64_t n)
{
	uint64_t i;
	for (i = 0; i < n; i++)
		;
}

void
endless_loop()
{
	while (1)
		//puts("Entre al endless_loop\n",0xff0000);
		;
}

void
endless_loop_print(uint64_t wait)
{
	int64_t pid = my_getpid();

	while (1) {
		char c[100];
		uint_to_base(pid, c, 10);
		puts(c,0xff00f0);

		bussy_wait(wait);
	}
}

// void*
// strcpy(void* destination, const void* source, uint64_t length)
// {
// 	/*
// 	 * memcpy does not support overlapping buffers, so always do it
// 	 * forwards. (Don't change this without adjusting memmove.)
// 	 *
// 	 * For speedy copying, optimize the common case where both pointers
// 	 * and the length are word-aligned, and copy word-at-a-time instead
// 	 * of byte-at-a-time. Otherwise, copy by bytes.
// 	 *
// 	 * The alignment logic below should be portable. We rely on
// 	 * the compiler to be reasonably intelligent about optimizing
// 	 * the divides and modulos out. Fortunately, it is.
// 	 */
// 	uint64_t i;

// 	if ((uint64_t)destination % sizeof(uint32_t) == 0 && (uint64_t)source % sizeof(uint32_t) == 0 &&
// 	    length % sizeof(uint32_t) == 0) {
// 		uint32_t* d = (uint32_t*)destination;
// 		const uint32_t* s = (const uint32_t*)source;

// 		for (i = 0; i < length / sizeof(uint32_t); i++)
// 			d[i] = s[i];
// 	} else {
// 		uint8_t* d = (uint8_t*)destination;
// 		const uint8_t* s = (const uint8_t*)source;

// 		for (i = 0; i < length; i++)
// 			d[i] = s[i];
// 	}

// 	return destination;
// }