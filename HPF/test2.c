#include <stdio.h>
#include <unistd.h>


#include "simu_posix.h"



int main(int argc, char* argv[])
{

	/* Initialise le simulateur */

	init_simu();



	/* Creation des taches aperiodiques */

	tache_aperiodique(3,3,6,"tache1");
	tache_aperiodique(3,0,0,"tache3");
	tache_aperiodique(4,1,0,"tache2");



	/* Simule l'ordonnancement sur 10 unites de temps */

	ordonnance(10);


	exit(0);


}



