#include "isolation.h"
#include "../hardware/hw.h"
#include "../sched_simple/sched.h"


void __attribute__ ((naked))  data_interrupt()
{
// Pour tuer le process
	__asm("srsdb sp!, #0x13");
	__asm("sub lr, lr, #4");	
    __asm("cps #0x13");
	DISABLE_IRQ();
	register uint32_t* errAddr;
	register uint8_t errType;
	__asm volatile("MRC p15, 0, %[errType], c5, c0, 0" : [errType] "=r" (errType));
	__asm volatile("MRC p15, 0, %[errAddr], c6, c0, 0" : [errAddr] "=r" (errAddr));
	errAddr+=1;
	errType+=1;
	kill_current_process();
	ENABLE_IRQ();
}
