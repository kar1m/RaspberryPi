#include "stdlib.h"
#include "./sched_fixed_priority/sched_fixed.h"
#include "./hardware/hw.h"
#include "./sem_pi/sem.h"

sem_s bloc,test;

void funcA()
{
    int cptA = 0;
	
    while (1) {
	int i,j;
	sem_down(&bloc);
	led_on();
	led_off();
    }
}

void funcB()
{

int clt =0;
while (1) {
	int i,j;
	sem_down(&test);
	led_on();
	led_off();
	clt++;

	}
}

void funcC()
{
sem_up(&test);
sem_up(&bloc);
int clt =0;
while (clt < 10) {
	int i,j;
	led_on();
	led_off();
	clt++;
    }
}

//------------------------------------------------------------------------
int kmain ( void )
{
  init_hw();
    sem_init(&bloc,0);
	sem_init(&test,0);
    current_process->pt_fct = NULL;
    int stack_size = STACK_SIZE;
    create_process_fixed(funcB, NULL, stack_size,LOW);
    create_process_fixed(funcA, NULL, stack_size,MEDIUM);
    create_process_fixed(funcC, NULL, stack_size,HIGH);

    start_sched();
   // ctx_switch();
    while(1){}
    /* Pas atteignable vues nos 2 fonctions */
    return 0;
}
