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
#define TOTAL_TT_SIZE (FIRST_LVL_TT_SIZE + SECON_LVL_TT_SIZE * FIRST_LVL_TT_COUN) //0x404000=4.210.688
#define PAGE_COUN (0x21000000/PAGE_SIZE)

//Adresses des tables (TABLES KERNEL)
#define FIRST_LVL_TT_START_ADDR 0x48000
#define SECON_LVL_TT_START_ADDR (FIRST_LVL_TT_SIZE + FIRST_LVL_TT_START_ADDR)
#define SECON_LVL_TT_END_ADDR (SECON_LVL_TT_START_ADDR + SECON_LVL_TT_SIZE * FIRST_LVL_TT_COUN) 
#define MMUTABLEBASE (FIRST_LVL_TT_START_ADDR)

//Flags Translation Table
#define KERNEL_FIRST_TT_FLAGS 0x1
#define PROCES_FIRST_TT_FLAGS KERNEL_FIRST_TT_FLAGS
#define KERNEL_SECON_TT_FLAGS_INIT 0x0052
#define KERNEL_SECON_TT_FLAGS_FREE 0x0072
#define KERNEL_SECON_TT_FLAGS_DEVICES 0x0017
#define PROCES_SECON_TT_FLAGS 0x0872

//Zone memoire Kernel
#define KERNEL_MRY_CMPL 0xC000
#define KERNEL_MRY_SIZE KERNEL_MRY_CMPL+TOTAL_TT_SIZE

#define KERNEL_MRY_ADDR FIRST_LVL_TT_START_ADDR

//Table d'occupation des mini-frames
#define MINI_FRAMES_SIZE_OF_A_FRAME 16
#define MINI_FRAMES_TOTAL_SIZE (KERNEL_MRY_SIZE/MINI_FRAMES_SIZE_OF_A_FRAME/8)
#define MINI_FRAMES_FRAMES_COUN MINI_FRAMES_TOTAL_SIZE*8

#define MINI_FRAMES_ADDR (KERNEL_MRY_ADDR+KERNEL_MRY_SIZE)

//Table d'occupation des frames
#define FRAMES_OCCUP_TABLE_ADDR (MINI_FRAMES_ADDR+MINI_FRAMES_TOTAL_SIZE)
#define FRAMES_OCCUP_TABLE_SIZE (0x21000000/PAGE_SIZE) //16896=0x4200

//Adresses limites de translation
#define NO_TRANS_BEG_ADDR1 0x0
#define NO_TRANS_END_ADDR1 (FRAMES_OCCUP_TABLE_SIZE+FRAMES_OCCUP_TABLE_ADDR)
#define NO_TRANS_END_ADDR1 0x500000
#define NO_TRANS_BEG_ADDR2 0x20000000
#define NO_TRANS_END_ADDR2 0x21000000
#define KERNEL_FREE_AREA_BEG 0x8000
#define KERNEL_FREE_AREA_END 0x40000

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

/* ATTENTION Division */
#define MINI_SIZE_TO_NB_PAGES(size) (size/MINI_FRAMES_SIZE_OF_A_FRAME+1)

//Donnees pour les programmes
#define INIT_STACK_POINTER 0x19FFFFFF
#define HEAP_BEG NO_TRANS_END_ADDR1
#define HEAP_END HEAP_BEG+SECON_LVL_TT_COUN*PAGE_SIZE*32


//Structure utilisee pour la detection d'espace libre
//en espace logique
struct FreeSpace;
struct FreeSpace {
    struct FreeSpace* ptNextFreeSpace;
	uint32_t* addrSpace;
    uint32_t  nbPagesFree;
};

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

extern void PUT32( unsigned int addr, unsigned int content);
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

uint32_t* Kernel_InitTTEntries();
/* Initialise la table de traduction du kernel et renvoie l'adresse
 * de la table primaire
 */

void vMem_Init();

void InitFirstEntries(uint32_t* primaryTableAddr);
/* Initialise pour la table primaire d'un processus placee a
 * primaryTableaAddr la non traduction des adresses systemes
 */
/*ATTENTION ! POUR L'INSTANT LES TRADUCTIONS VERS LES @PERIPHERIQUES
 * SONT DESACTIVEES (TRANSLATION FAULT)
 */

/* -------------- ESPACE VIRTUEL
 * Fonctions permettant de reserver des ressources virtuelles
 * de maniere coherente
 */
FreeSpace* VirtualSpace_Find(uint32_t nbPages);

uint32_t* VirtualSpace_Get(uint32_t nbPages);
/* Alloue un bloc contigu d'adresses LOGIQUES dans l'espace
 * memoire de l'instance courante
 * Renvoie le pointeur sur la premiere adresse si celle-ci a ete
 * trouvee, NULL sinon
 */

void VirtualSpace_Fill(FreeSpace* freeSpace, uint32_t nbPages);

void VirtualSpace_Release(uint32_t* logAddrToRealease, uint32_t nbPages);

FreeSpace* VirtualSpace_GetPrevious(FreeSpace* freeSpace);

FreeSpace* VirtualSpace_GetNextFreeSpace(uint32_t* logAddrToRealease);


/* -------------- UTILITAIRES
 *
 */
#define DVD_ZERO_ERROR 0
#define DVD_SUCCES 1
uint8_t Divide(uint32_t top, uint32_t btm, uint32_t* ptRslt, uint32_t* ptMod);


#endif
