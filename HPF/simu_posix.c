#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "simu_posix.h"

/* Simule l'horloge du systeme : l'horloge est incr�ment�e
   a chaque pas de l'ordonnanceur */
int horloge;


/* Liste des taches pretes (cf fichier file_de_taches.h) */
struct file_de_taches   pretes;


/* Liste des taches bloquees (cf fichier file_de_taches.h) */
struct file_de_taches   bloquees;



/* Initialise le simulateur */
void init_simu(void)
{
	int diag=0;

	horloge=0;

	diag=init_file_de_taches(&pretes);
	if(diag<0)
		printf("PB initialisation file de taches pretes\n");

	diag=init_file_de_taches(&bloquees);
	if(diag<0)
		printf("PB initialisation file de taches bloquees\n");

}



/* Creer une tache aperiodique */
void tache_aperiodique(int capacite, int priorite, 
		int prete_a_partir_de, char* nom)
{

	int diag;
	struct desc_tache ma_tache;

	ma_tache.capacite=capacite;
	ma_tache.priorite=priorite;
	ma_tache.prete_a_partir_de=prete_a_partir_de;
	ma_tache.reste_a_executer=capacite;
	ma_tache.cpu_consommee=0;
	strcpy(ma_tache.nom,nom);


	/* Si la tache arrive plus tard qu'a l'instant zero, c'est quelle
	   est bloquee */

	if(ma_tache.prete_a_partir_de>0)
		{
		diag=inserer_file_de_taches_ordonnees(&bloquees, ma_tache);
		if(diag<0)
			printf("PB inserer_file_de_taches bloquees\n");
		}
	else	{
		diag=inserer_file_de_taches(&pretes, ma_tache);
		if(diag<0)
			printf("PB inserer_file_de_taches pretes\n");
		}

}



/* Lance le simulateur : on effectue l'ordonnancement
sur "pas" unites de temps */

void ordonnance(int pas)
{
	int i;

	for(i=0;i<pas;i++)
		faire_un_pas();
}



/* Ordonnance le systeme sur une unite de temps */
void faire_un_pas(void)
{

	int i=0, prio=-1, ind=-1;
	int diag;
	struct desc_tache ma_tache;


	/* On regarde s'il existe des taches a reveiller : passer de l'etat bloque
	   a l'etat pret  */
	
	while( (bloquees.taches[bloquees.tete].prete_a_partir_de <= horloge)
		&& (bloquees.nombre_de_taches!=0) )
		{
		diag=retirer_file_de_taches(&bloquees, &ma_tache);
		if(diag<0)
			printf("PB retirer_file_de_taches bloquees\n");

		diag=inserer_file_de_taches(&pretes, ma_tache);
		if(diag<0)
			printf("PB inserer_file_de_taches pretes\n");
		}



	/* On recherche la tache prete de plus haute priorit� */

	i=pretes.tete; 
	while(i!=pretes.queue)
		{
		if( (pretes.taches[i].priorite>prio) 
			&& (pretes.taches[i].reste_a_executer>0) )
			{
			prio=pretes.taches[i].priorite;
			ind=i;
			}

		if(i==(MAX_TACHES-1))
			i=0;
		else	i++;
		}


	if(ind<0)
		printf("A l'instant %d, pas de tache prete\n",horloge);
	else	{

		/* On fait avancer la tache */
		printf("A l'instant %d, %s execute une unite de temps\n", horloge, pretes.taches[ind].nom);


		pretes.taches[ind].cpu_consommee++;
	
		pretes.taches[ind].reste_a_executer--;
		}
	horloge++;
}




