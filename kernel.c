#include "stdlib.h"
#include "./hardware/hw.h"
#include "./sem_pi/sem.h"
#include "./sched_simple/sched.h"
#include "syscall/syscall.h"
#include "VirtualMemory/vmem.h"
#include "pwm.h"

void funcA()
{
    int cptA = 0;

    while (1) {
		int j = 0; 
		int i = 0; 

		for (i = 0 ; i < 5; i++) 
		{
			for (j = 0; j< 10000; j++);
			led_on();
			for (j = 0; j< 10000; j++);
			led_off();
		}
    }
}

void funcB()
{

	int clt =0;
	while (1) {
		int j = 0; 
		int i = 0; 

		for (i = 0 ; i < 20; i++) 
		{
			for (j = 0; j< 100000; j++);
			led_on();
			for (j = 0; j< 100000; j++);
			led_off();
		}
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

   /*
 init_hw();

	testVM();
    int stack_size = STACK_SIZE;
    create_process(funcB, NULL, stack_size);
    create_process(funcA, NULL, stack_size);
    create_process(funcC, NULL, stack_size);

    start_sched( SIMPLE );
*/
	audio_test();

    while(1){}
    /* Pas atteignable vues nos 2 fonctions */
    return 0;
}
