#include <stdio.h>
#include <unistd.h>


#include "simu_posix.h"



int main(int argc, char* argv[])
{


	/* Initialise le simulateur */

	init_simu();



	/* Creation des taches aperiodiques */

	tache_aperiodique(3,0,0,"tache1");
	tache_aperiodique(3,1,0,"tache2");
	tache_aperiodique(3,3,0,"tache3");



	/* Simule l'ordonnancement sur 11 unites de temps */

	ordonnance(11);



	exit(0);

}



