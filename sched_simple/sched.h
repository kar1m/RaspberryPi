#ifndef SCHED_H
#define SCHED_H

#define STACK_SIZE 1024 

#include "../VirtualMemory/vmem.h"
#include "stdlib.h"

typedef void (*func_t) (void*);

typedef enum pcb_state {
	RUNNING,
	TERMINATED,
	READY,
	WAITING,
	SLEEPING,
	CREATED
} pcb_state;

typedef enum pcb_priority {
	HIGH,
	MEDIUM,
	LOW
} pcb_priority;

typedef enum sched_policy
{
	SIMPLE,
	FIXED
} sched_policy; 


typedef struct pcb_s{
	uint8_t id;					//Id processus
	pcb_state ps_state;			//Etat du processus
	func_t pt_fct;				//Pointeur de fonction
	void* pt_args;				//Pointeur vers les arguments
	uint32_t* firstTTaddr;		//Adresse de la table de translation de niveau 1
	FreeSpace*	pt_fsHeap;		//Pointeur vers le premier descripteur d'espace libre du tas
	struct pcb_s* pt_nextPs;	//Pointeur vers le processus suivant
	unsigned int currentSP;		//Pointeur de pile courant
	unsigned int currentPC;		//Compteur d'instruction courant
	unsigned int stackSize;		//Stack size pour dépiler
	pcb_priority priority;		// PRIORITE
	unsigned int nbQuantums;
} pcb_s;

struct pcb_s* current_process; 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<SCHED_FIXED>>>>>>>>>>>>>>>>>>>>>>>>>>>>>//
pcb_priority current_priority;
#define NUMBER_LISTS 3 
struct pcb_s* tab_files_process [NUMBER_LISTS]; 




//void init_pcb(struct pcb_s* pcb, func_t f, void* ptArgs, pcb_priority priority);
//void create_process(func_t f, void* args, unsigned int stack_size,pcb_priority priority);
void init_pcb(pcb_s* pcb, func_t f, void* ptArgs);
void create_process(func_t f, void* args, unsigned int stack_size);


void start_sched( sched_policy _policy );
void elect();

void start_current_process();
void kill_current_process();

void MMU_commutation(uint32_t* newFirstTTAddr, uint8_t newASID);
/* Permet d'effectuer le changement de table primaire */

void ctx_switch_from_irq();
unsigned int cpu_cycles();



#endif
