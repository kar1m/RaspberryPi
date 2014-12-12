#include "stdlib.h"
#include "./sched_simple/sched.h"
#include "./hardware/hw.h"
//#include "syscall/syscall.h"

void funcA()
{
    int cptA = 0;

    while (1) {
		int i,j;
		for (i=0; i<200000; i++);
		led_on();
		for (j=0; j<200000; j++);
		led_off();
    }
}

void funcB()
{

int clt =0;
	while (1) {
		int i,j;
		for (i=0; i<100000; i++);
		led_on();
		for (j=0; j<100000; j++);
		led_off();
		clt++;
	}
}

void funcC()
{
int clt =0;
	while (clt < 10) {
		int i,j;
		for (i=0; i<100000; i++);
		led_on();
		for (j=0; j<100000; j++);
		led_off();
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
