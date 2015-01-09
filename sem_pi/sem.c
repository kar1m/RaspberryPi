#include "sem.h"

void sem_init(struct sem_s* sem, int val)
{
	sem->compteur = val;
}

void sem_up(struct sem_s* sem)
{
	sem->compteur = sem->compteur + 1;
	if (sem->compteur <= 0)
	{
		// Déblocage d'un processus dans la file d'attente
		sem->fileAttente[0]->ps_state = READY;
		sem->fileAttente[0]->blocked = 0;
		// Réaménagement de la file d'attente
		int index = 0;
		pcb_s* next = sem->fileAttente[1]; 
		while (next != NULL)
		{
			sem->fileAttente[index++] = sem->fileAttente[index];
			next = sem->fileAttente[index+1]; 
		}
	} 
}

void sem_down(struct sem_s* sem)
{
	sem->compteur = sem->compteur - 1;
	if (sem->compteur < 0)
	{
		// Blocage du processus appelant 
		current_process->blocked = 1;
		sem->fileAttente[sizeFile++] = current_process;
		while (current_process->blocked == 1)
		{
			sys_wait(1000);
		}
	}
}

void mtx_init(struct mtx_s* mutex)
{
	mutex->compteur = 1;
}
void mtx_lock(struct mtx_s* mutex)
{
	
}
void mtx_unlock(struct mtx_s* mutex)
{

}

