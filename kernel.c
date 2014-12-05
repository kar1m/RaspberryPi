#include "stdlib.h"
#include "sched.h"
#include "hw.h"
#include "VirtualMemory/vmem.h"

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
    while ( 1 ) {
        cptB += 2 ;
       // ctx_switch();
    }
}

void funcC(int babar)
{
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
    	init_kern_translation_table();
	generateTestValues();
	configure_mmu_C();
	start_mmu_C();
	testVM();	
	return 0;
}
