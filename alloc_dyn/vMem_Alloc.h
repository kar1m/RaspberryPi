#ifndef VMEM_ALLOC_H
#define VMEM_ALLOC_H

#include <stdint.h>


//Structure utilisee pour la detection d'espace libre
//en espace logique
struct FreeSpace;
typedef struct FreeSpace {
    struct FreeSpace* ptNextFreeSpace;
	uint32_t* addrSpace;
    uint32_t  nbPagesFree;
} FreeSpace;

/* -------------------- VMEM_ALLOC --------------------
 * 
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

void vMem_Init();

/* ------------------ ESPACE VIRTUEL -------------------
 * Fonctions permettant de reserver des ressources virtuelles
 * de maniere coherente
 * s'appuie sur la structure FreeSpace qui represente un bloc de
 * memoire VIRTUELLE
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


#endif
