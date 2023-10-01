#ifndef IDT_LOADER_H
#define IDT_LOADER_H

#include <stdint.h>

/*
 * Interpreta la direccion de la IDT (0x00) como un arreglo de Entradas a la IDT.
 * Cada entrada tiene entre sus campos, el puntero a la rutina wrapper que se
 * encarga de llamar a los dispatchers.
 */
void idt_loader();

#endif
