#ifndef SCHED_H
#define SCHED_H

#define STACK_SIZE 1024 

#include "stdlib.h"
#include "../sched_fixed_priority/sched_fixed.h"

typedef struct sem_s{
	unsigned int compteur; //Compteur de semaphore
	pcb_s** fileAttente; //file d'attente
} sem_s;

void sem_init(struct sem_s* sem, unsigned int val);

#endif
