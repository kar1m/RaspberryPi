#include "sched.h"
#include "../alloc_simple/phyAlloc.h"
#include "stdlib.h"
#include "../VirtualMemory/vmem.h"
#include "../VirtualMemory/Mini_Alloc.h"
#include "../hardware/hw.h"
#include "../alloc_dyn/vMem_Alloc.h"
#include "stdint.h"

/* Variables globales pour la MMU */
uint8_t Kernel_ASID = 1;
extern uint32_t* Kernel_FirstTTAddr;
FreeSpace* ptFirstFreeSpace;

/* 																/
------------------------------Utils------------------------------
/																*/
unsigned int cpu_cycles() { 
	unsigned int cycles;
	__asm volatile ("mrc p15, 0, %0, c15, c12, 1" : "=r" (cycles));
	return cycles;
}			
/* 																	    /
------------------------------Ordonnanceur------------------------------
/																	  */	



void init_pcb(struct pcb_s* pcb, func_t f, void* ptArgs)
{
	static uint8_t nbProcess = 2;	//On demarre à 2, histoire que l'OS ait un ASID (1)
	pcb->ps_state = CREATED;
	pcb->pt_fct = f;
	pcb->pt_args = ptArgs;	
	pcb->id = nbProcess++;
}
//<<FIXED>>//
/*
void init_pcb(struct pcb_s* pcb, func_t f, void* ptArgs, pcb_priority priority)
{
	static int nbProcess = 0;
	pcb->ps_state = CREATED;
	pcb->pt_fct = f;
	pcb->pt_args = ptArgs;	
	pcb->id = nbProcess++;
	pcb->priority = priority;
}*/


void create_process(func_t f, void* args, unsigned int stack_size)
{
	//On libère de la place pour notre pcb DANS L'ESPACE KERNEL
	struct pcb_s* pt_newPcb = (pcb_s*) Mini_Alloc(MINI_SIZE_TO_NB_PAGES(sizeof(pcb_s)),0);
	init_pcb(pt_newPcb,f,args);
	pt_newPcb->stackSize = stack_size;
	pt_newPcb->firstTTaddr = CreateMemoryArea();
	pt_newPcb->pt_fsHeap = (FreeSpace*) Mini_Alloc(MINI_SIZE_TO_NB_PAGES(sizeof(FreeSpace)),0);
	
	//On vérifie si c'est le premier
	if(pt_newPcb->id == 0) {
		current_process = pt_newPcb;
		current_process->pt_nextPs = pt_newPcb;
	}
		

	//Mise à jour last pcb pour ajouter le nouveau pcb à la liste
	pcb_s* pt_nextPcb = current_process->pt_nextPs;
	pt_newPcb->pt_nextPs = pt_nextPcb;
	current_process->pt_nextPs=pt_newPcb; 		
       
	//Allocation mémoire pour la pile et le PC
	pt_newPcb->currentPC = (unsigned int) f;
	pt_newPcb->currentSP = INIT_STACK_POINTER;
		//Deplacement du SP pour le mettre au sommet de la pile, 
		//en comptant les 13 registres à placer au dessus (48)
		pt_newPcb->currentSP -= 52;
	
	pt_newPcb->nbQuantums = 0; 
}
//<<FIXED>>//
/*void create_process(func_t f, void* args, unsigned int stack_size,pcb_priority priority)
{
	//On libère de la place pour notre pcb
	struct pcb_s* pt_newPcb = (pcb_s*) phyAlloc_alloc(sizeof(pcb_s));
	init_pcb(pt_newPcb,f,args,priority);
	pt_newPcb->stackSize = stack_size;	
	
	//Allocation mémoire pour la pile et le PC
	pt_newPcb->currentPC = (unsigned int) f;
        pt_newPcb->currentSP = (unsigned int) phyAlloc_alloc(stack_size*4);
        //Deplacement du SP pour le mettre au sommet de la pile, 
        //en comptant les 13 registres à placer au dessus (48)
	pt_newPcb->currentSP += (stack_size*4-52);

	pt_newPcb->nbQuantums = 0; 
	//Mise à jour last pcb pour ajouter le nouveau pcb à la liste
	struct pcb_s* first_process_priority = tab_files_process[priority];
	if(first_process_priority == NULL)
	{
		tab_files_process[priority] = pt_newPcb;
	}else{
		struct pcb_s* current_process_priority = first_process_priority;	
		while( current_process_priority->pt_nextPs != NULL)
		{
			current_process_priority = current_process_priority->pt_nextPs; 
		}
		current_process_priority->pt_nextPs = pt_newPcb;
	}
}
*/

void kill_current_process_simple()
{
	//On referme la boucle
	struct pcb_s* scanned_process = current_process;

	//On balaye toute la boucle afin de
	//recuperer l'element avant le 
	//process courant, afin de lui assigner un nw suivant
	while(scanned_process->pt_nextPs != current_process)
		scanned_process = scanned_process->pt_nextPs;
	scanned_process->pt_nextPs = current_process->pt_nextPs;		
	
	//Liberation memoire 
	void* pt_stack = (void*)(current_process->currentSP-current_process->stackSize+52);
	vMem_Free((uint32_t*)pt_stack,PAGE_SIZE);	//
	Mini_Free((uint32_t*)current_process->firstTTaddr, MINI_SIZE_TO_NB_PAGES(sizeof(FreeSpace)));	//
	Mini_Free((uint32_t*)current_process, MINI_SIZE_TO_NB_PAGES(sizeof(pcb_s)));	//
	

	current_process = scanned_process->pt_nextPs;
}

void kill_current_process_fixed()
{
	tab_files_process[current_priority] = tab_files_process[current_priority]->pt_nextPs;


	//Liberation memoire 
	void* pt_stack = (void*)(current_process->currentSP-current_process->stackSize+52);
	phyAlloc_free(pt_stack,current_process->stackSize);
	phyAlloc_free(current_process, sizeof(pcb_s));
}

void elect_simple()
{
	current_process = current_process->pt_nextPs;
}
void elect_fixed()
{
	int i = 0;
	for(i=0; i<NUMBER_LISTS; i++)
	{
		struct pcb_s* first_process_priority = tab_files_process[i];
		if(first_process_priority != NULL)
		{	
			current_process = first_process_priority;
			current_priority = i; 
			break;
		}
	}
}

void start_sched_simple()
{	
	//On cree un nouveau pcb afin d'ammorcer la pompe	
	struct pcb_s* pt_newPcb = (pcb_s*) Mini_Alloc(MINI_SIZE_TO_NB_PAGES(sizeof(pcb_s)),0);
	init_pcb(pt_newPcb,NULL,NULL);
	//On fait pointer le champ "nextPs" de notre nouveau 
	//processus vers le premier de notre liste
	pt_newPcb->pt_nextPs = current_process;
	current_process = pt_newPcb;
       
	//Allocation mémoire pour la pile et le PC
	pt_newPcb->currentPC = 0;
	pt_newPcb->currentSP = (unsigned int) Mini_Alloc(MINI_SIZE_TO_NB_PAGES(48*4),0);	//! #DEFINE 
	//Note : On va peut etre pas s'embeter a creer de la memoire virtuelle alors qu'on s'en sert pas
    pt_newPcb->currentSP += (52*4-4);
    
    ENABLE_IRQ();
    set_tick_and_enable_timer();
}

void start_sched_fixed()
{	
    	ENABLE_IRQ();
   	set_tick_and_enable_timer();
}

//------------Ordonnanceur simple---------------

void __attribute__ ((naked)) switch_next()
{
	
	//1 Sauvegarde du contexte
	//----on sauvegarde les registres
	__asm("push {r0-r12}");
	register unsigned int previousSP;
	register unsigned int previousPC;
	//----cad on range le PC courant dans la pile
	__asm("mov %0, lr" : "=r"(previousPC));
	//----puis on sauvegarde le SP
	__asm("mov %0, sp" : "=r"(previousSP));
	
	__asm("mcr p15, 0, r0, c8, c6, 0"); //c7 au lieu de c6?? 
	__asm("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (Kernel_FirstTTAddr));
	__asm("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (Kernel_FirstTTAddr));
	__asm("MCR p15, 0, %[asid], c13, c0, 1" : : [asid] "r" (Kernel_ASID));

	current_process->currentPC = previousPC;
	current_process->currentSP = previousSP;
	//2 Changement de contexte
	//----cad on change de contexte courant
	current_process = current_process->pt_nextPs;
	register unsigned int currentSP = current_process->currentSP;
	register unsigned int currentPC = current_process->currentPC;

	__asm("mcr p15, 0, r0, c8, c6, 0"); //c7 au lieu de c6?? 
	__asm("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (current_process->firstTTaddr));
	__asm("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (current_process->firstTTaddr));
	__asm("MCR p15, 0, %[asid], c13, c0, 1" : : [asid] "r" (current_process->id));

	//----et on recharge le SP
	__asm("mov sp, %0" : : "r"(currentSP));
	//----on charge LR pour le retour
	__asm("mov lr, %0" : : "r"(currentPC));
	//----on restaure les registres
	__asm("pop {r0-r12}");
	
	//----puis on branche sur le PC du nouveau contexte
	__asm("bx lr");
	
}





/**
------------------INTERFACE---------------------
**/
sched_policy policy = 0 ; 

void start_sched(sched_policy _policy) 
{
	policy = _policy; 

	if(policy== SIMPLE)
	{
		start_sched_simple();
	}else
	{
		start_sched_fixed();
	}
}


void start_current_process()
{
	register func_t pt_fct = current_process->pt_fct;
	register void* pt_args = current_process->pt_args;
	
	//Commutation MMU Kernel->Processus
	__asm("mcr p15, 0, r0, c8, c6, 0"); //c7 au lieu de c6?? 
	__asm("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (current_process->firstTTaddr));
	__asm("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (current_process->firstTTaddr));
	__asm("MCR p15, 0, %[asid], c13, c0, 1" : : [asid] "r" (current_process->id));

	pt_fct(pt_args);
}

void kill_current_process()
{
	if(policy== SIMPLE )
	{
		kill_current_process_simple();
	}else
	{
		kill_current_process_fixed();
	}
}


void elect()
{
	if(policy== SIMPLE )
	{
		elect_simple();
	}else
	{
		elect_fixed();
	}
}

void ctx_switch_from_irq()
{
    DISABLE_IRQ();
    __asm("sub lr, lr, #4");
    __asm("srsdb sp!, #0x13");
    __asm("cps #0x13");
    
    //Sauvegarde du contexte
    
    //----on sauvegarde les registres
    __asm("push {r0-r12}");

	//Commutation MMU vers Kernel
	__asm("mcr p15, 0, r0, c8, c6, 0"); //c7 au lieu de c6?? 
	__asm("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (Kernel_FirstTTAddr));
	__asm("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (Kernel_FirstTTAddr));
	__asm("MCR p15, 0, %[asid], c13, c0, 1" : : [asid] "r" (Kernel_ASID));

    //----cad on range le PC courant dans la pile
    __asm("mov %0, lr" : "=r"(current_process->currentPC));
    //----puis on sauvegarde le SP
    __asm("mov %0, sp" : "=r"(current_process->currentSP));
    
    //Changement etat processus
    if (current_process->ps_state != SLEEPING) {
        current_process->ps_state = READY;
    }

    //2 Election
    int continue_elect = 1;
    while(continue_elect)
    {
        elect();
        switch(current_process->ps_state)
        {
            case CREATED:
                current_process->ps_state = RUNNING;
		
				set_tick_and_enable_timer();
				ENABLE_IRQ();		

                start_current_process();
                current_process->ps_state = TERMINATED;
                break;
                
            case TERMINATED:
                kill_current_process();
                break;
                
            case SLEEPING: 
				if(current_process->nbQuantums > 0)
				{
					current_process->nbQuantums--;
					break;
				}else {
					current_process->ps_state = READY;
				}
            case READY:
                current_process->ps_state = RUNNING;
				//3A Sauvegarde du SP et du PC
				register unsigned int currentSP = current_process->currentSP;
				register unsigned int currentPC = current_process->currentPC;

                //3B Commutation MMU vers Processus
				__asm("mcr p15, 0, r0, c8, c6, 0"); //c7 au lieu de c6?? 
				__asm("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (current_process->firstTTaddr));
				__asm("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (current_process->firstTTaddr));
				__asm("MCR p15, 0, %[asid], c13, c0, 1" : : [asid] "r" (current_process->id));

                //3C Restauration contexte
                //----et on recharge le SP
                __asm("mov sp, %0" : : "r"(currentSP));
                //----on charge LR pour le retour
                __asm("mov lr, %0" : : "r"(currentPC));
                //----on restaure les registres
				set_tick_and_enable_timer();
                __asm("pop {r0-r12}");

				ENABLE_IRQ();

				__asm("rfeia sp!");                

                continue_elect = 0;
                break;
                
            default:
                continue_elect = 1;
                break;
        }
        
    }
}
