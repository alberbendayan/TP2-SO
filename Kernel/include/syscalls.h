#ifndef SYSCALLS
#define SYSCALLS

#include <stdint.h>

/*
 * Maneja las syscalls, basicamente una wrapper que llama a los distintos drivers.
 */
uint64_t syscall_dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r9, uint64_t r8);

/*
 * Recibe el puntero al stack donde se encuentran pusheados los valores de los registros,
 * junto con los valores pusheados automáticamente ante una interrupción.
 */
void save_registers(uint64_t* stack);

#endif
