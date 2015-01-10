#ifndef MINI_H
#define MINI_H

#include <stdint.h>
#include "VMem_Constants.h"
//Pour l'@table, voir VMem_Constants.h

/* ATTENTION Division */
#define MINI_SIZE_TO_NB_PAGES(size) (size/MINI_FRAMES_SIZE_OF_A_FRAME+1)

/* ------------------ MINI-FRAMES-TABLE
 * Table d'occupation de la zone des
 * tables primaires et secondaires
 * ATTENTION : UNE TABLE PRIMAIRE OCCUPE
 * 16 FOIS PLUS DE PLACE QU'UNE TABLE
 * SECONDAIRE
 * */

void Mini_SetOccupied(uint32_t noPage);
/* Passer la page noPage a occupee
*/

void Mini_SetUnoccupied(uint32_t noPage);
/* Passer la page noPage a libre
*/

int32_t Mini_FindFirstUnoccupied(uint32_t noPageDebutRecherche);
/* Trouver la premiere page libre a partir de la page
 * noPageDebutRecherche
 */

int32_t Mini_FindFirstOccupied(uint32_t noPageDebutRecherche);
/* Trouver la premiere page occupee a partir de la page
 * noPageDebutRecherche
 */

int Mini_CheckRangeOccupation(uint32_t noPageDebut, uint32_t noPageFin);
/* Renvoie 1 si au moins une page du range (noPageFin non inclue)
 * est occupee,
 * Renvoie 0 sinon (toutes les pages sont libres)
 * OBSOLETE
 */

int Mini_CheckOccupation(uint32_t noPage);
/* Renvoie 1 si la page est occupee
 * Renvoie 0 si la page est libre
 */

void Mini_InitFramesTable();
/* Initialise une petite table des frames pour les tables de translation
 * ATTENTION : afin d'optimiser l'allocation, les Mini_-frames sont de taille
 */



uint32_t* Mini_Alloc(uint32_t nbMiniFrames, uint8_t nbZeros);
/* Alloue dans la zone des tables primaires et secondaires l'espace
 * pour nbMiniFrames et renvoie l'adresse de debut, le parametre nbZeros indique le nombre
 * de bits qui doivent etres a zero en bout d'adresse
 * (si allocation classique : mettre 0)
 */

uint8_t Mini_Free(uint32_t* ptTable, uint32_t nbMiniFrames);
/* Desalloue dans la zone des tables primaires et secondaires l'espace
 * pour nbMiniFrames et renvoie l'adresse de debut
 */

uint32_t* Mini_NoPageToPageLineAdress(uint32_t noPage);


#endif