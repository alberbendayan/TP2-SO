#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdint.h>

/*
 * Wrapper de las funciones que manejan las excepciones.
 */
void exception_dispatcher(uint32_t exception, uint64_t* stack);

/*
 * Imprime los Registros en pantalla en caso de excepción y comando 'printreg'
 */
void exc_printreg(uint64_t* stack, uint32_t color);

/*
 * Restaura el puntero al Stack y reinicia la shell (IP = 0x400000)
 */
void exc_set_restore_point(uint64_t ip, uint64_t sp, uint64_t bp);

#endif
