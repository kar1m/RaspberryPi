

#ifndef SIMU_H
#define SIMU_H



#include "file_de_taches.h"






/******************************/
/* Fonctions du simulateur    */
/******************************/


/* Initialise le simulateur */
void init_simu(void);



/* Creer une tache aperiodique */
/* Une tache a un nom symbolique et est definie par 
une capacite, une priorite et une date a partir de laquelle
elle devient prete (ex : date d'arrivee dans 
le systeme) */
void tache_aperiodique(int capacite, int priorite, 
		int prete_a_partir_de, char* nom);


/* Ordonnance le systeme de "pas"  unites de temps */
void ordonnance(int pas);


/* Ordonnance le systeme sur une unite de temps */
void faire_un_pas(void);



#endif 
