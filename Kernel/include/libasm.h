/*
 * Estas funciones están definidas en libasm/libasm.asm
 */

#ifndef LIBASM_H
#define LIBASM_H

#include <stdint.h>
#include <process.h>

/*
 * Devuelve la información del productor del procesador.
 */
extern char* asm_cpu_vendor(char* result);

/*
 * Devuelve el fmt (HORA, DIA, etc..) indicado del tiempo medido por RTC.
 */
extern int asm_rtc_gettime(int fmt);

/*
 * Devuelve si hay un caracter disponible, es decir, si se presionó alguna tecla.
 */
extern char asm_kbd_active();

/*
 * Devuelve la tecla que haya sido presionada cuando saltó la interrupción.
 */
extern int asm_kbd_getkey();

/*
 * Devuelve el stack pointer actual.
 */
extern uint64_t asm_getsp();

/*
 * Devuelve el base pointer actual.
 */
extern uint64_t asm_getbp();

/*
 * Imprime los registros cargados en el arreglo de la snapshot.
 */
extern void asm_printreg(uint32_t color);

/*
 * Reproduce un sonido por el pcspkr con la frecuencia deseada.
 */
extern void asm_sound(uint32_t freq);

/*
 * Frena la reproducción de sonido.
 */
extern void asm_nosound();

/*
 * Genera una interrupcion del timertick
 */
extern void asm_timertick();

/*
 * mueve el rsp a una direccion arbitraria
 */
extern void asm_move_rsp(uint64_t rsp);

extern void asm_idle();

extern void asm_initialize_stack(void (*f) (main_function code, char ** args),main_function,void*,void*);

/*
 * Devuelve la información del productor del procesador.
 */
extern int asm_xchg(int *mutex, int val);
#endif
