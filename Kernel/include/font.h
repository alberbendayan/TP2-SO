#ifndef FONT_H
#define FONT_H

#include <stdint.h>

#define CHAR_WIDTH 8
#define CHAR_HEIGHT 16

/*
 * Toma la letra que queres escribir y te devuelve la posición en el bitmap correspondiente,
 * los siguientes 16 bits en el arreglo configuran la skin de tu caracter.
 */
uint8_t* fnt_get_char_bitmap(char c);

#endif
