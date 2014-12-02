#include "syscall.h"

void sys_reboot()
{
	__asm("SWI 0" : : : "lr");
}

void SWIHandler()
{
	
}
