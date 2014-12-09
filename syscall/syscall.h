#ifndef SYSCALL_H
#define SYSCALL_H

#include "stdlib.h"

void sys_reboot();
void SWIHandler();
void doSysCallReboot();
void sys_wait(unsigned int nbQuantums);
void doSysCallWait(unsigned int nbQuantums);

#endif
