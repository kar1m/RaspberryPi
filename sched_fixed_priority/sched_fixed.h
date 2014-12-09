#ifndef SCHED_FIXED_H
#define SCHED_FIXED_H

#define STACK_SIZE 1024 

#include "stdlib.h"

typedef void (*func_t) (void*);

typedef enum pcb_priority {
	HIGH,
	MEDIUM,
	LOW
} pcb_priority;

typedef enum pcb_state {
	RUNNING,
	TERMINATED,
	READY,
	WAITING,
	SLEEPING,
	CREATED
} pcb_state;

typedef struct pcb_s{
	int id;			//Id processus
	pcb_state ps_state;	//Etat du processus
	func_t pt_fct;		//Pointeur de fonction
	void* pt_args;		//Pointeur vers les arguments
	struct pcb_s* pt_nextPs;//Pointeur vers le processus suivant
	unsigned int currentSP;	//Pointeur de pile courant
	unsigned int currentPC;	//Compteur d'instruction courant
	unsigned int stackSize;//Stack size pour dépiler
	pcb_priority priority;
	unsigned int nbQuantums;
} pcb_s;


struct pcb_s* current_process;
pcb_priority current_priority;

#define NUMBER_LISTS 3 
struct pcb_s* tab_files_process [NUMBER_LISTS]; 


void init_pcb(struct pcb_s* pcb, func_t f, void* ptArgs, pcb_priority priority);
/* Permet d'initialiser le PCB d'une fonction
 *
 */
void create_process_fixed(func_t f, void* args, unsigned int stack_size,pcb_priority priority);
void start_current_process();
void elect();
void start_sched();
void kill_current_process();
void ctx_switch_from_irq();

#endif
