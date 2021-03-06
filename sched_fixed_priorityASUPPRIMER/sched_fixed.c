#include "sched_fixed.h"
#include "../hardware/hw.h"
#include "../alloc_simple/phyAlloc.h"
#include "stdlib.h"

/* Ordonnanceur*/
void init_pcb(struct pcb_s* pcb, func_t f, void* ptArgs, pcb_priority priority)
{
	static int nbProcess = 0;
	pcb->ps_state = CREATED;
	pcb->pt_fct = f;
	pcb->pt_args = ptArgs;	
	pcb->id = nbProcess++;
	pcb->priority = priority;
}

void create_process_fixed(func_t f, void* args, unsigned int stack_size,pcb_priority priority)
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

void start_current_process()
{
	//Lancement de la fonction du ps courant avec ses args
	current_process->pt_fct(current_process->pt_args);
}

void kill_current_process()
{
	tab_files_process[current_priority] = tab_files_process[current_priority]->pt_nextPs;


	//Liberation memoire 
	void* pt_stack = (void*)(current_process->currentSP-current_process->stackSize+52);
	phyAlloc_free(pt_stack,current_process->stackSize);
	phyAlloc_free(current_process, sizeof(pcb_s));
}
void elect()
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

void start_sched()
{	
    	ENABLE_IRQ();
   	set_tick_and_enable_timer();
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
    //----cad on range le PC courant dans la pile
    __asm("mov %0, lr" : "=r"(current_process->currentPC));
    //----puis on sauvegarde le SP
    __asm("mov %0, sp" : "=r"(current_process->currentSP));
    
    //Changement etat processus
    current_process->ps_state = READY;
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

		
	    case WAITING:
		continue_elect = 1;
                break;
            	
            case READY:
                current_process->ps_state = RUNNING;
                
                //3 Restauration contexte
                //----et on recharge le SP
                __asm("mov sp, %0" : : "r"(current_process->currentSP));
                //----on charge LR pour le retour
                __asm("mov lr, %0" : : "r"(current_process->currentPC));
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

