#ifndef FILE_DE_TACHES_H
#define FILE_DE_TACHES_H

/* Fichier offrant des services pour inserer et retirer des 
   taches dans une file  */

/* Nombre maximum de taches dans une file d'attente */
#define MAX_TACHES 10



/* Descripteur de tache : contient toutes les informations
			  necessaires pour une tache        */
typedef struct desc_tache {
	/* Nom de la tache */
	char nom [100];

	/* Priorite de la tache */
	int priorite;

	/* Capacite de la tache */
	int capacite;

	/* Somme du temps CPU utilise par la tache  */
	double cpu_consommee;

        /* Date a partir de laquelle la tache est prete
                (ex : arrivee dans le systeme)  */
        int prete_a_partir_de;
	
	/* Capacite restant a executer */
	int reste_a_executer;	

	/* Periode de la tache */
	int periode;	


} desc_tache;
		


/****************************************************/
/*            Gestion des files de taches           */
/****************************************************/



/* La structure "file_de_taches" est une file d'attente.  */

typedef struct file_de_taches {


		/* Le tableau "taches" permet de contenir MAX_TACHES 
  		   descripteurs de taches  */
		struct desc_tache taches[MAX_TACHES+1];


		/* "tete" pointe sur la tete de la liste */
		int tete;


		/* "queue" pointe sur la prochaine position libre 
		   (cad la case memoire qui sera utilisee 
		   lors de la prochaine insertion */
		int queue;


		/* Nombre de taches dans la file */
		int nombre_de_taches;

} file_de_taches;



/* Initialise la file d'attente "f" */
int init_file_de_taches(struct file_de_taches *f);


/* Permet d'inserer dans la file d'attente "f" le
   descripteur de tache "t". L'insertion est faite
   en queue de la file  */
int inserer_file_de_taches(struct file_de_taches *f, 
		struct desc_tache t);


/* Permet d'inserer dans la file d'attente "f" le
   descripteur de tache "t" par ordre croissant
   du champ "prete_a_partir_de"    */
int inserer_file_de_taches_ordonnees(struct file_de_taches *f,
                struct desc_tache t);


/* Permet de retirer de la file d'attente "f" le
   descripteur de tache "t". On retire en tete de file  */
int retirer_file_de_taches(struct file_de_taches *f, 
		struct desc_tache *t);


/* Permet de retirer de la file d'attente "f" le
   descripteur de tache "t". On retire l'element à l'endroit "position"  */
int retirer_file_de_taches_position(struct file_de_taches *f, 
		struct desc_tache *t,
		int position);


#endif 
