#ifndef PHYLOS_H
#define PHYLOS_H

#include <stdint.h>


typedef enum
{
    THINKING = 1,
    EATING = 2,
    HUNGRY = 3
    
} state;

typedef struct philosopher
{
    int pid;
    int sem;
    state philo_state;

} philosopher;

void run_philos(int argc, char *argv[]);



#endif
