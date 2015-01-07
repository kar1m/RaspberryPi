#include "stdlib.h"
#include "./hardware/hw.h"
#include "./sem_pi/sem.h"
#include "./sched_simple/sched.h"
#include "syscall/syscall.h"
#include "VirtualMemory/vmem.h"


extern uint32_t* Kernel_FirstTTAddr;

void funcA()
{
    int cptA = 0;

    while (1) {
	cptA++;
	ctx_switch_from_irq();
    }
}

void funcB()
{

	int clt =0;
	while (1) {
		clt += 5;
		ctx_switch_from_irq();

	}
}

void funcC()
{
	int clt =0;
	while (clt < 10) {
		clt++;
		ctx_switch_from_irq();

    }
}

//------------------------------------------------------------------------
int kmain ( void )
{
	current_process = (pcb_s*) Mini_Alloc(MINI_SIZE_TO_NB_PAGES(sizeof(pcb_s)),0);
	current_process->pt_fct = NULL;
	// SEQUENCE INITIALISATION
    init_hw();
	ConfigureKTT_And_EnableMMU();

	testVM();
    int stack_size = STACK_SIZE;
    create_process(funcB, NULL, stack_size);
    create_process(funcA, NULL, stack_size);
    create_process(funcC, NULL, stack_size);

    start_sched( SIMPLE );
	start_current_process();
	start_current_process();
    while(1){}
    /* Pas atteignable vues nos 2 fonctions */
    return 0;
}
