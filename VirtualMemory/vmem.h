#ifndef VMEM_H
#define VMEM_H

#include <stdint.h>
#include "VMem_Constants.h"

//Pour les @TT, voir VMem_Constants.h

//Flags Translation Table
#define KERNEL_FIRST_TT_FLAGS 0x1
#define PROCES_FIRST_TT_FLAGS KERNEL_FIRST_TT_FLAGS
#define KERNEL_SECON_TT_FLAGS_INIT 0x0852
#define KERNEL_SECON_TT_FLAGS_FREE 0x0072
#define KERNEL_SECON_TT_FLAGS_DEVICES 0x0017
#define PROCES_SECON_TT_FLAGS 0x0872

//Erreurs ajout table de translation
#define ADD_TT_SUCCESS 1
#define ADD_TT_ERR_LOGICAL_ADDR_EXISTING 10

//Tables de translations, utilitaires
#define PRIMARY_TRANS_FAULT_1 0x3
#define PRIMARY_TRANS_FAULT_2 0x0
#define SECONDARY_TRANS_FAULT 0x0

#define IS_PRIMARY_TRANS_FAULT(descriptor) ( ((((uint32_t)descriptor) << 30) == (PRIMARY_TRANS_FAULT_1 << 30)) || ((((uint32_t) descriptor) << 30) == (PRIMARY_TRANS_FAULT_2 << 30)) )

#define IS_SECONDARY_TRANS_FAULT(descriptor)  ((((uint32_t)descriptor) << 30) == (SECONDARY_TRANS_FAULT << 30))

#define GET_PRIMARY_ENTRY_ADDR(primaryTableAddr,logicalAddr)	( (uint32_t)primaryTableAddr + ((( (uint32_t)logicalAddr )>>18)&0x3FFC) )

#define GET_SECONDARY_ENTRY_ADDR(secondaryTableAddr,logicalAddr)	( (uint32_t)secondaryTableAddr + ((( (uint32_t)logicalAddr )>>10)&0x3FC) )

#define GET_SECONDARY_TABLE_ADDR(primaryDescr) (0xFFFFC00 & primaryDescr)

//Donnees pour les programmes
#define INIT_STACK_POINTER 0x19FFFFFF
#define HEAP_BEG NO_TRANS_END_ADDR1
#define HEAP_END HEAP_BEG+SECON_LVL_TT_COUN*PAGE_SIZE*32



typedef struct FreeSpace FreeSpace;

void ConfigureKTT_And_EnableMMU();
/* Rempli la TT du Kernel et active la MMU */

unsigned int init_kern_translation_table();
/* Initialise la table de translation de l'OS
 * Laisse @logique = @physique pour les espaces
 * reserves a l'OS
 * Cette manoeuvre est a effectuer avant la
 * configuration et le demarrage de la MMU
 */

void generateTestValues();
int testVM();
/* Fonctions de Test de la memoire virtuelle
 */

void start_mmu_C(); 
void configure_mmu_C(uint32_t* primaryTableAddr);
/* Fonctions (fournies) de demarrage et de config
 * de la MMU
 */

uint32_t* CreateMemoryArea();
/* Cree une nouvelle table de translation avec trois tables de niveau 2
 * initialisees, une pour le code, une pour le pile et l'autre pour le tas
 * L'adresse retournee est celle de la table de niveau 1
 */

uint8_t LinkLogAddrToPhyAddr(	uint32_t* physicalAddr,
								uint32_t* primaryTableAddr, 
								uint32_t* desirededLogicalAddr,
								uint32_t primaryFlags,
								uint32_t secondaryFlags,
								uint8_t rewriteIfExisting);
/* Permet d'ajouter a une table de translation une nouvelle page
 * ayant pour @physique physicalAddr
 * et pour @logique desirededLogicalAddr
 * Renvoie ADD_TT_SUCCESS si l'ecriture s'est bien passee
 * Renvoie ADD_TT_ERR_LOGICAL_ADDR_EXISTING si l'adresse logique
 *	existait deja mais effectue quand meme l'operation si rewriteIfExisting
 *	est a 1
 */

uint32_t* GetPhyFromLog(uint32_t* primaryTableAddr, uint32_t* logAddr);
/* Permet de recuperer l'adresse de l'emplacement physique
 * associe a une page
 */



uint32_t* Kernel_InitTTEntries();
/* Initialise la table de traduction du kernel et renvoie l'adresse
 * de la table primaire
 */


void InitFirstEntries(uint32_t* primaryTableAddr);
/* Initialise pour la table primaire d'un PROCESSUS placee a
 * primaryTableaAddr la non traduction des adresses systemes
 */
/*ATTENTION ! POUR L'INSTANT LES TRADUCTIONS VERS LES @PERIPHERIQUES
 * SONT DESACTIVEES (TRANSLATION FAULT)
 */

#endif
