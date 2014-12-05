#include "stdlib.h"
#include "sched_fixed.h"
#include "hw.h"

void funcA()
{
    int cptA = 0;
    while ( 1 ) {
        cptA ++;
    //    ctx_switch();
    }
}

void funcB()
{
    int cptB = 1;
    while ( cptB < 200 ) {
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
    create_process_fixed(funcB, NULL, stack_size,LOW);
    create_process_fixed(funcA, NULL, stack_size,LOW);
    create_process_fixed(funcC, NULL, stack_size,MEDIUM);
    start_sched();
   // ctx_switch();
    while(1){}
    /* Pas atteignable vues nos 2 fonctions */
    return 0;
}
