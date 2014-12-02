#include <stdio.h>
#include "file_de_taches.h"



/****************************************************/
/*            Gestion des files de taches           */
/****************************************************/

/* La structure "file_de_taches" est une file d'attente. 
- Le tableau "taches" permet de contenir MAX_TACHES 
  descripteurs de taches 
- "tete" pointe sur la tete de la liste
- "queue" pointe sur la prochaine position libre (cad la 
   case memoire qui sera utilisee lors de la prochaine insertion */



/* Initialise la file d'attente "f" */

int init_file_de_taches(struct file_de_taches *f)
{
	f->tete=0;
	f->queue=0;
	f->nombre_de_taches=0;
	
	return 0;
}



/* Permet d'inserer dans la file d'attente "f" le
   descripteur de tache "t". L'insertion est faite
   en queue de la file  */

int inserer_file_de_taches(struct file_de_taches *f, 
	struct desc_tache t)
{

	if(f->tete == ((f->queue+1)%(MAX_TACHES+1)) )
		{
		printf("La file de tache est pleine ...\n");
		return -1;
		}

	f->taches[f->queue]=t;

	if(f->queue==MAX_TACHES)
		f->queue=0;
	else	f->queue++;

	f->nombre_de_taches++;

	return 0;
}





/* Permet d'inserer dans la file d'attente "f" le 
   descripteur de tache "t" par ordre croissant
   du critere "ordre"    */
int inserer_file_de_taches_ordonnees(struct file_de_taches *f,
                struct desc_tache t)
{

	int courant=f->nombre_de_taches;
	int ptr=f->queue;

	if(f->nombre_de_taches==0)
		{
		inserer_file_de_taches(f,t);
		return 0;
		}

        if(f->tete == ((f->queue+1)%(MAX_TACHES+1)) )
                {
                printf("La file de tache est pleine ...\n");
                return -1;
                }

	/* On se positionne en fin de liste */
	/* On remonte la liste jqa a la tete en
	   decalant les elements d'une place, 
	   lorsque la propriete d'ordre devient vrai ...
	   on insere   */


	while(courant!=0)
		{
		/* On decale */
	        if(ptr==0)
	        	ptr=MAX_TACHES;
	        else    ptr--;
		
		if(f->taches[ptr].prete_a_partir_de > t.prete_a_partir_de)
			{
			int destination=ptr;

        		if(destination==MAX_TACHES)
        		        destination=0;
        		else    destination++;

			f->taches[destination]=f->taches[ptr];
			}
		else	{
        		if(ptr==MAX_TACHES)
        		        ptr=0;
        		else    ptr++;
			break;
			}

		courant--;		
		}


	/* On insere */
        f->taches[ptr]=t;

	/* Ne pas oublier d'augmenter la queue */
        if(f->queue==MAX_TACHES)
                f->queue=0;
        else    f->queue++;

        f->nombre_de_taches++;

        return 0;

}



/* Permet de retirer de la file d'attente "f" le
   descripteur de tache "t". On retire en tete de file  */

int retirer_file_de_taches(struct file_de_taches *f, 
	struct desc_tache* t)
{
	if(f->tete==f->queue)
		{
		printf("La file de taches est vide ...\n");
		return -1;
		}

	*t=f->taches[f->tete];

	if(f->tete==MAX_TACHES)
		f->tete=0;
	else	f->tete++;


	f->nombre_de_taches--;

	return 0;
}


/* Permet de retirer de la file d'attente "f" le
   descripteur de tache "t". On retire l'element à l'endroit "position"  */
int retirer_file_de_taches_position(struct file_de_taches *f,
                struct desc_tache *t,
                int position)
{
	int ptr;

	if(f->tete==f->queue)
		{
		printf("La file de taches est vide ...\n");
		return -1;
		}

	*t=f->taches[position];

	/* On decale les elements entre "tete" et "position" */
	while(position!=f->tete)
		{
		ptr=position;

        	if(position==0)
        		position=MAX_TACHES;
        	else    position--;

		f->taches[ptr]=f->taches[position];
		}

	if(f->tete==MAX_TACHES)
		f->tete=0;
	else	f->tete++;

	f->nombre_de_taches--;

	return 0;
}
