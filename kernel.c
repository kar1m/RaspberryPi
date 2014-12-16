#include "stdlib.h"
#include "./hardware/hw.h"
//#include "./sem_pi/sem.h"
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

// SEQUENCE INITIALISATION
	/*init_kern_translation_table();
	configure_mmu_C();
	start_mmu_C();
	vmem_init();
	unsigned char* p1, * p2, * p3;
	p1 = vmem_alloc(10);
	p2 = vmem_alloc(5);
	vmem_free(p1, 10);
	p3 = vmem_alloc(10);*/

    init_hw();
     

    int stack_size = STACK_SIZE;
    create_process(funcB, NULL, stack_size);
    create_process(funcA, NULL, stack_size);
    create_process(funcC, NULL, stack_size);

    start_sched();

    while(1){}
    /* Pas atteignable vues nos 2 fonctions */
    return 0;
}
