#include "isolation.h"
#include "../hardware/hw.h"
#include "../sched_simple/sched.h"


void data_handler()
{
// Pour tuer le process
	kill_current_process();
}
