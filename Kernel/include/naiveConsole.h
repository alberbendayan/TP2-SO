#ifndef NAIVE_CONSOLE_H
#define NAIVE_CONSOLE_H

#include <stdint.h>

void nc_print(const char * string);
void nc_putchar(char character);
void nc_newline();
void nc_print_dec(uint64_t value);
void nc_print_hex(uint64_t value);
void nc_print_bin(uint64_t value);
void nc_print_base(uint64_t value, uint32_t base);
void nc_print_time();
void nc_backspace();
void nc_set_color(unsigned char fg, unsigned char bg);
void nc_clear();
void nc_clear_color();

#endif
