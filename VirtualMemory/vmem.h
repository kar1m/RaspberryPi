#ifndef VMEM_H
#define VMEM_H

#include <stdint.h>


//Definition des constantes de pagination
#define PAGE_SIZE 4096
#define SECON_LVL_TT_COUN 256
#define SECON_LVL_TT_SIZE 1024
#define FIRST_LVL_TT_COUN 4096 
#define FIRST_LVL_TT_SIZE 16384
#define SECTION_SIZE (SECON_LVL_TT_COUN * PAGE_SIZE)
#define TOTAL_TT_SIZE (FIRST_LVL_TT_SIZE + SECON_LVL_TT_SIZE * FIRST_LVL_TT_COUN)
#define PAGE_COUN (0x21000000/PAGE_SIZE)

//Adresses des tables
#define FIRST_LVL_TT_START_ADDR 0x48000
#define SECON_LVL_TT_START_ADDR (FIRST_LVL_TT_SIZE + FIRST_LVL_TT_START_ADDR)
#define SECON_LVL_TT_END_ADDR (SECON_LVL_TT_START_ADDR + SECON_LVL_TT_SIZE * FIRST_LVL_TT_COUN)
#define MMUTABLEBASE (FIRST_LVL_TT_START_ADDR)

//Adresses limites de translation
#define NO_TRANS_BEG_ADDR1 0x0
#define NO_TRANS_END_ADDR1 0x700000
#define NO_TRANS_BEG_ADDR2 0x20000000
#define NO_TRANS_END_ADDR2 0x21000000

//Table d'occupation des frames
#define FRAMES_OCCUP_TABLE_ADDR 0x4FBE00
#define FRAMES_OCCUP_TABLE_SIZE 4808

//Erreurs ajout table de translation
#define ADD_TT_SUCCESS 1
#define ADD_TT_ERR_LOGICAL_ADDR_EXISTING 10

//Table d'occupation des mini-frames
#define FRAMES_TT_SIZE_OF_A_FRAME SECON_LVL_TT_SIZE
#define FRAMES_TT_SECON_TABLE_OCCUP SECON_LVL_TT_SIZE/FRAMES_TT_SIZE_OF_A_FRAME
#define FRAMES_TT_FIRST_TABLE_OCCUP FIRST_LVL_TT_SIZE/FRAMES_TT_SIZE_OF_A_FRAME
#define FRAMES_TT_ADDR 0x4FB000
#define FRAMES_TT_TOTAL_SIZE (FRAMES_TT_ADDR - FIRST_LVL_TT_START_ADDR)/FRAMES_TT_SIZE_OF_A_FRAME/8
#define FRAMES_TT_FRAMES_COUN FRAMES_TT_TOTAL_SIZE*8


//Tables de translations, utilitaires
#define PRIMARY_TRANS_FAULT_1 0x3
#define PRIMARY_TRANS_FAULT_2 0x0
#define SECONDARY_TRANS_FAULT 0x0

#define IS_PRIMARY_TRANSLATION_FAULT(descriptor) ( ((((uint32_t)descriptor) << 30) == (PRIMARY_TRANS_FAULT_1 << 30)) || ((((uint32_t) descriptor) << 30) == (PRIMARY_TRANS_FAULT_2 << 30)) )

#define IS_SECONDARY_TRANSLATION_FAULT(descriptor)  ((((uint32_t)descriptor) << 30) == (PRIMARY_TRANS_FAULT_1 << 30))

#define GET_PRIMARY_ENTRY_ADDR(primaryTableAddr,logicalAddr)	( primaryTableAddr + (((uint32_t)logicalAddr)>>20) )

#define GET_SECONDARY_ENTRY_ADDR(secondaryTableAddr,logicalAddr)	( secondaryTableAddr + ((( (uint32_t)logicalAddr )>>12)&0xFF) )

#define GET_SECONDARY_TABLE_ADDR(primaryDescr) (0xFFFFC00 & primaryDescr)


//Donnees pour les programmes
#define INIT_STACK_POINTER 0x20FFFFFF
#define HIPE_START 0x50000
#define HIPE_END HIPE_START+SECON_LVL_TT_COUN*PAGE_SIZE*32


//Structure utilisee pour la detection d'espace libre
//en espace logique
struct FreeSpace {
    uint32_t* ptNextFreeSpace;
    uint32_t  nbPagesFree;
};

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
void configure_mmu_C();
/* Fonctions (fournies) de demarrage et de config
 * de la MMU
 */

uint32_t* vMem_Alloc(unsigned int nbPages);
/* Cherche nbPages en memoire virtuelle
 * et renvoie le pointeur vers l'espace
 * ATTENTION : renvoie l'adresse virtuelle
 */

void vMem_Free(uint32_t* ptr, unsigned int nbPages);
/* Libere nbPages allouees en memoire derriere
 * le pointeur ptr
 * ATTENTION : le pointeur est celui de l'espace virtuel
 */

extern void PUT32( unsigned int, unsigned int);
/* Fonction permettant d'ajouter manuellement 32 bits en
 * memoire
 */

/* -----------------------FRAMES-TABLE
 * Table d'occupation de la memoire
 * physique, les zones systemes sont
 * ignorees
 * */

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








uint32_t* CreateMemoryArea();
/* Cree une nouvelle table de translation avec trois tables de niveau 2
 * initialisees, une pour le code, une pour le pile et l'autre pour le tas
 * L'adresse retournee est celle de la table de niveau 1
 */

uint8_t UpdateEntryInSecondaryTable(uint32_t* physicalAddr,
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



/* ------------------ MINI-FRAMES-TABLE
 * Table d'occupation de la zone des
 * tables primaires et secondaires
 * ATTENTION : UNE TABLE PRIMAIRE OCCUPE
 * 16 FOIS PLUS DE PLACE QU'UNE TABLE
 * SECONDAIRE
 * */

void MiniSetOccupied(uint32_t noPage);
/* Passer la page noPage a occupee
*/

void MiniSetUnoccupied(uint32_t noPage);
/* Passer la page noPage a libre
*/

int32_t MiniFindFirstUnoccupied(uint32_t noPageDebutRecherche);
/* Trouver la premiere page libre a partir de la page
 * noPageDebutRecherche
 */

int32_t MiniFindFirstOccupied(uint32_t noPageDebutRecherche);
/* Trouver la premiere page occupee a partir de la page
 * noPageDebutRecherche
 */

int MiniCheckRangeOccupation(uint32_t noPageDebut, uint32_t noPageFin);
/* Renvoie 1 si au moins une page du range (noPageFin non inclue)
 * est occupee,
 * Renvoie 0 sinon (toutes les pages sont libres)
 * OBSOLETE
 */

int MiniCheckOccupation(uint32_t noPage);
/* Renvoie 1 si la page est occupee
 * Renvoie 0 si la page est libre
 */

void MiniInitFramesTable();
/* Initialise une petite table des frames pour les tables de translation
 * ATTENTION : afin d'optimiser l'allocation, les mini-frames sont de taille
 */

uint32_t* MiniAlloc(uint8_t nbMiniFrames);
/* Alloue dans la zone des tables primaires et secondaires l'espace
 * pour nbMiniFrames et renvoie l'adresse de debut
 */

uint8_t MiniFree(uint32_t* ptTable, uint8_t nbMiniFrames);
/* Desalloue dans la zone des tables primaires et secondaires l'espace
 * pour nbMiniFrames et renvoie l'adresse de debut
 */
#endif
