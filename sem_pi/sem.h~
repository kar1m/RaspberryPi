#ifndef SEM_H
#define SEM_H

#include "stdlib.h"
#include "../sched_fixed_priority/sched_fixed.h"

typedef struct sem_s{
	int compteur; //Compteur de semaphore
	pcb_s** fileAttente; //file d'attente
} sem_s;

typedef struct mtx_s{
	int compteur; //Compteur de mutex
	pcb_s** fileAttente; //file d'attente
} mtx_s;

//SEMAPHORES
void sem_init(struct sem_s* sem, int val);
void sem_up(struct sem_s* sem);
void sem_down(struct sem_s* sem);
//MUTEX
void mtx_init(struct mtx_s* mutex);
void mtx_lock(struct mtx_s* mutex);
void mtx_unlock(struct mtx_s* mutex);

int sizeFile;
#endif
