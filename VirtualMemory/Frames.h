#ifndef FRAMES_H
#define FRAMES_H

#include <stdint.h>


/* TABLE DES FRAMES
 * Le mecanisme de frames permet, par l'intermediaire
 * des fonctions ci-dessous, de savoir si une page PHYSIQUE
 * (bloc de 0x1000=4096 octets de memoire) est occupee ou non
 * Pour eviter tout conflit, seules les pages entre 0x500 000
 * et 0x20 000 000 sont considerees comme non occupees a 
 * l'initialisation
 *
 * ATTENTION : il est necessaire d'appeler initFramesTable() 
 * avant toute utilisation des fonctions
 */

void setOccupied(uint32_t noPage);
/* Passer la page noPage a occupee
*/

void setUnoccupied(uint32_t noPage);
/* Passer la page noPage a libre
*/

int32_t findFirstUnoccupied(uint32_t noPageDebutRecherche);
/* Trouver la premiere page libre a partir de la page 
 * noPageDebutRecherche
 */

int32_t findFirstOccupied(uint32_t noPageDebutRecherche);
/* Trouver la premiere page occupee a partir de la page 
 * noPageDebutRecherche
 */

int checkRangeOccupation(uint32_t noPageDebut, uint32_t noPageFin);
/* Renvoie 1 si au moins une page du range (noPageFin non inclue)
 * est occupee,
 * Renvoie 0 sinon (toutes les pages sont libres)
 * OBSOLETE
 */

int checkOccupation(uint32_t noPage);
/* Renvoie 1 si la page est occupee
 * Renvoie 0 si la page est libre
 */

void initFramesTable();
/* Initialise la table des frames a l'adresse FRAMES_OCCUP_TABLE_ADDR
 * en marquant comme occupees les pages de l'OS
 */

uint32_t* getPageDescriptor(uint32_t noPage);
/* Trouver le descripteur de second niveau de la page 
 * noPage
 */

uint32_t* noPageToPageLineAdress(uint32_t noPage);
/* Renvoie l'adresse de la ligne (de 8 bits) sur laquelle se trouve
 * le bit occupe/libre de la page specifiee 
 */

#endif