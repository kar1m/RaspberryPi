#include "stdlib.h"
#include "./sched_simple/sched.h"
#include "syscall/syscall.h"

void funcA()
{
    int cptA = 0;
    while (1) {
	cptA++;
    }
}

void funcB()
{

	int clt =0;
	while (1) {
		clt += 5;
	}
}

void funcC()
{
	int clt =0;
	while (clt < 10) {
		clt++;
    }
}

//------------------------------------------------------------------------
int kmain ( void )
{
    init_hw();
     
    current_process->pt_fct = NULL;
    int stack_size = STACK_SIZE;
    create_process(funcB, NULL, stack_size);
    create_process(funcA, NULL, stack_size);
    create_process(funcC, NULL, stack_size);

    start_sched();
   	// ctx_switch();
    while(1){}
    /* Pas atteignable vues nos 2 fonctions */
    return 0;
}
