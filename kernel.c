#include "stdlib.h"
#include "./hardware/hw.h"
#include "./sem_pi/sem.h"
#include "./sched_simple/sched.h"
#include "./syscall/syscall.h"
#include "./VirtualMemory/vmem.h"
#include "./VirtualMemory/Mini_Alloc.h"

extern uint32_t* Kernel_FirstTTAddr;

void funcA()
{
    int cptA = 0;

    while (1) {
		cptA+=2;
		switch_next();
		//A
    }
}

void funcB()
{
	int clt =0;
	while (1) {
		clt+=5;
		switch_next();
		//B
	}
}

void funcC()
{
	uint32_t* pt = (uint32_t*)0x200000;
	*pt = 12345678;
	int clt =0;
	while (clt < 10) {
		clt++;
		switch_next();
		//C
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

	//testVM();
    int stack_size = STACK_SIZE;
    create_process(funcB, NULL, stack_size);
    create_process(funcA, NULL, stack_size);
    create_process(funcC, NULL, stack_size);

    //start_sched( SIMPLE );
	funcB();
    while(1){}
    /* Pas atteignable vues nos 2 fonctions */
    return 0;
}
