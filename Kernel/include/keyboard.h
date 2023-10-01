#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

/*
 * Maneja la interrupción de hardware del teclado. Llena el buffer del teclado
 * y guarda los valores en orden de inserción. Realiza el handle para la combinación
 * Ctrl+r que realiza la carga de los registros.
 */
int keyboard_handler();

/*
 * Devuelve el siguiente caracter del buffer y el estado del mismo (presionado
 * o soltado).
 */
char kb_getchar(uint8_t* state);

#endif
