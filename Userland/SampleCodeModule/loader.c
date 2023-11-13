// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <loader.h>

extern char bss;
extern char endOfBinary;

extern int main();


int
_start()
{
	// Clean BSS
	memset(&bss, 0, &endOfBinary - &bss);
	return main();
}

void*
memset(void* destiation, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char* dst = (char*)destiation;

	while (length--) {
		dst[length] = chr;
	}

	return destiation;
}
