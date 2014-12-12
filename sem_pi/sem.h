#ifndef SCHED_H
#define SCHED_H

#define STACK_SIZE 1024 

#include "stdlib.h"

typedef struct sem_s{
	unsigned int compteur; //Compteur de semaphore
	unsigned int currentSP; //Pointeur de pile courant
} sem_s;


#endif
