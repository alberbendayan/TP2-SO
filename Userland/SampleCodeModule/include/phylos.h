#ifndef PHYLOS_H
#define PHYLOS_H

#include <stdint.h>


typedef enum
{
    THINKING = 1,
    EATING = 2,
    HUNGRY = 3
    
} state;

typedef struct phylosopher
{
    int pid;
    int sem;
    int id_phylo;
    state philo_state;

} phylosopher;

void run_phylos(int argc, char* argv[]);



#endif
