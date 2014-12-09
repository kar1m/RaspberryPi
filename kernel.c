#include "stdlib.h"
#include "./sched_simple/sched.h"
#include "./hardware/hw.h"
//#include "syscall/syscall.h"

void funcA()
{
    int cptA = 0;
    while ( 1 ) {
        cptA ++;
    	//sys_wait(3000);
	//    ctx_switch();
    }
}

void funcB()
{
    int cptB = 1;
    while (1) {
        cptB += 2 ;
       // ctx_switch();
    }
}

void funcC()
{
int babar =0;
    while(babar<37)
    {
        babar++;
      //  ctx_switch();
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
