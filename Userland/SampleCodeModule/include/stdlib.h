#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>

/*
 * Devuelve el caracter presionado por teclado y su estado
 * (presionado o soltado).
 */
uint8_t getchar(uint8_t* state);

/*
 * Carga en buff los caracteres recibidos por teclado y los
 * imprime por pantalla.
 */
uint32_t gets(char* buff, uint32_t size, uint32_t color);

/*
 * Imprime un caracter por pantalla.
 */
void putchar(char c, uint32_t color);

/*
 * Imprime un string por pantalla.
 */
void puts(char* str, uint32_t color);

/*
 * Devuelve el largo de un string.
 */
uint64_t strlen(char* buff);

/*
 * Parsea un string por token seleccionado.
 */
uint32_t strtok(char* buff, uint8_t token, char** args, uint32_t size);

/*
 * Compara dos strings.
 */
uint32_t strcmp(char* s1, char* s2);

/*
 * Transforma un entero en un string formateado en cierta base.
 */
uint32_t uint_to_base(uint64_t value, char* buff, uint32_t base);

/*
 * Devuelve si un string es un código hexadecimal válido.
 */
uint8_t is_hex_color_code(char* code);

/*
 * Transforma un código hexadecimal en entero.
 */
uint32_t hex_to_uint(char* code);

#endif
