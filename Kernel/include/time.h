#ifndef TIME_H
#define TIME_H

#include <stdint.h>

/*
 * Maneja la interrupción de hardware del Timer Ticks
 */
void timer_handler();

/*
 * Retorna la cantidad de ticks contados
 */
uint64_t ti_ticks();

/*
 * Retorna la cantidad de segundos que pasaron desde la ejecución
 * del programa. 18 ticks equivale a 1 segundo.
 */
uint64_t ti_seconds();

/*
 * Retorna true si el tick saltó y false si todavía no.
 * Usado para manejar el framerate en el pong.
 */
uint8_t ti_ticked();

/*
 * Genera una demora durante una cierta cantidad de ticks.
 */
void ti_sleep(uint32_t ticks);

#endif
