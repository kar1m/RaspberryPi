#include "vmem.h"
#include <stdio.h>

/* VARIABLES GLOBALES */

//Variables pot de fleur
unsigned int total=0;	//Pour start MMU
uint32_t globMod=0;		//Modulo global (pour la division)

//MMU 
uint32_t* Kernel_FirstTTAddr;
extern uint8_t Kernel_ASID;

//Inutile ??
FreeSpace* ptFirstFreeSpace;

/* REALISATION DE FONCTIONS */

unsigned int init_kern_translation_table()
{
/*	---- DEPRECATED since Isolation Phase  -----
 *	---- use Kernel_InitTTEntries instead  -----
 * Initialisation de la table de
 * niveau 1, à partir de l'adresse
 * specifiee
 *
 * ALGO :
 * On remplit la table de premier niveau entrée par entrée.
 * Pour les adresses traduites, on crée à chaque fois la table
 * de second niveau associée
 * Pour les adresses non translatees, l'entree associée dans la table
 * de premier niveau vaut 0 (translation fault)
 *
 * Pour cela, on parcours la table des adresses de 1er niveau
 * on calcule la premiere adresse virtuelle associee à cette zone
 * on charge dans la table de premier niveau le pointeur (en fait le "numero"
 *  de la table de niveau 2 associee (currentSecondaryTable_beg_addr)
 * on charge ensuite successivement les entrees des tables de niveau 2
 *  à l'endroit qui va bien
 *
 */
    uint32_t primaryTable_flag = 0x1;
	uint32_t primaryTable_transErrorFlag = 0x0;
    uint32_t device_flag = 0x0017;
    uint32_t normal_flag = 0x0052;

	//Adresse de la table de second niveau actuelle
	uint32_t currentSecondaryTable_beg_addr = SECON_LVL_TT_START_ADDR;
	uint32_t addr;
    for(addr=FIRST_LVL_TT_START_ADDR; addr<FIRST_LVL_TT_START_ADDR+FIRST_LVL_TT_SIZE; addr+=4 /*On remplit 32bits par 32bits*/)
	{			
		//Translation d'adresse
		//On determine la case de
		//la table de premier niveau
        uint32_t currentVirtualAddr = (addr-FIRST_LVL_TT_START_ADDR)<<18;
        uint32_t primaryTableEntry;
        int32_t addrInSecTable;
        if(	(currentVirtualAddr >= NO_TRANS_BEG_ADDR1 &&
            currentVirtualAddr < NO_TRANS_END_ADDR1) )
		{
			//Dans cette zone les adresses physiques
			// correspondent aux adresses logiques
            primaryTableEntry = (currentSecondaryTable_beg_addr)|primaryTable_flag;

            //On charge la table des pages de second niveau
            for(addrInSecTable = currentSecondaryTable_beg_addr;
				addrInSecTable<currentSecondaryTable_beg_addr+SECON_LVL_TT_SIZE;
				addrInSecTable+=4)
			{
                //La translation de 10 (au lieu de 12) evite la division par 4
                uint32_t secTableEntry = currentVirtualAddr
                                          | ((addrInSecTable-currentSecondaryTable_beg_addr) << 10)
                                          | normal_flag;
				PUT32(addrInSecTable,secTableEntry);
			}
			//On met a jour l'adresse de la table de second niveau :
			//comme on vient de remplir la table precedente, on pousse
			//l'adresse de la taille d'une table
            currentSecondaryTable_beg_addr += SECON_LVL_TT_SIZE;

        } else if(currentVirtualAddr >= NO_TRANS_BEG_ADDR2 &&
                  currentVirtualAddr < NO_TRANS_END_ADDR2) {
            //Dans cette zone les adresses physiques
            // correspondent aux adresses logiques
            primaryTableEntry = (currentSecondaryTable_beg_addr)|primaryTable_flag;

            //On charge la table des pages de second niveau
            for(addrInSecTable = currentSecondaryTable_beg_addr;
                addrInSecTable<currentSecondaryTable_beg_addr+SECON_LVL_TT_SIZE;
                addrInSecTable+=4)
            {
                //La translation de 10 (au lieu de 12) evite la division par 4
                uint32_t secTableEntry = currentVirtualAddr
                                          | ((addrInSecTable-currentSecondaryTable_beg_addr) << 10)
                                          | device_flag;
                PUT32(addrInSecTable,secTableEntry);
            }
            //On met a jour l'adresse de la table de second niveau :
            //comme on vient de remplir la table precedente, on pousse
            //l'adresse de la taille d'une table
            currentSecondaryTable_beg_addr += SECON_LVL_TT_SIZE;

        } else {
            primaryTableEntry=primaryTable_transErrorFlag;
		}
        PUT32(addr, primaryTableEntry);
	}
	return 0;
}

void generateTestValues()
{
	PUT32(0x4FFFFC, 12344321);
    PUT32(0x20000000, 20121992);
}

int testVM()
{
	/*uint32_t* pt1 = VirtualSpace_Get(1);
	uint32_t* pt2 = VirtualSpace_Get(10);
	VirtualSpace_Release(pt1,1);
	uint32_t* pt3 = VirtualSpace_Get(1);
	VirtualSpace_Release(pt3,1);
	uint32_t* pt4 = VirtualSpace_Get(15);
	VirtualSpace_Release(pt2,10);
	uint32_t* pt5 = VirtualSpace_Get(0x45000000);
	uint32_t* pt6 = VirtualSpace_Get(2000);*/

    return 0;
}

void ConfigureKTT_And_EnableMMU()
{
	initFramesTable();
	Kernel_FirstTTAddr = Kernel_InitTTEntries();
	configure_mmu_C(Kernel_FirstTTAddr);
	start_mmu_C();
}

void start_mmu_C()
{
	register unsigned int control;
	__asm("mcr p15, 0, %[zero], c1, c0, 0" : : [zero] "r"(0)); //Disable cache
	__asm("mcr p15, 0, r0, c7, c7, 0"); //Invalidate cache (data and instructions) */
	__asm("mcr p15, 0, r0, c8, c7, 0"); //Invalidate TLB entries
/* Enable ARMv6 MMU features (disable sub-page AP) */
	control = (1<<23) | (1 << 15) | (1 << 4) | 1;
/* Invalidate the translation lookaside buffer (TLB) */
	__asm volatile("mcr p15, 0, %[data], c8, c7, 0" : : [data] "r" (0));
/* Write control register */
	__asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));

	//Define the kernel as the current running process
	register uint8_t asid = Kernel_ASID;
	__asm volatile("MCR p15, 0, %[asid], c13, c0, 1" : : [asid] "r" (asid));
}

void configure_mmu_C(uint32_t* primaryTableAddr)
{
	register unsigned int pt_addr = (uint32_t)primaryTableAddr;
	total++;
/* Translation table 0 */
	__asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (pt_addr));
/* Translation table 1 */
	__asm volatile("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (pt_addr));
/* Use translation table 0 for everything */
	__asm volatile("mcr p15, 0, %[n], c2, c0, 2" : : [n] "r" (0));
/* Set Domain 0 ACL to "Manager", not enforcing memory permissions
 * * Every mapped section/page is in domain 0
 * */
	__asm volatile("mcr p15, 0, %[r], c3, c0, 0" : : [r] "r" (0x3));
}

uint32_t* vMem_Alloc(unsigned int nbPages)
/* Algo : 
 * 1> On cherche de l'espace logique
 * 2> On cherche de l'espace physique
 *      a chaque fois on linke l'espace logique avec l'espace physique
 */
{
    /* On raisonne en PAGE, mais on renvoie une adresse */
    uint32_t* addrMem = VirtualSpace_Get(nbPages);
	
	uint32_t currentPageNb;
	uint32_t currentFrameNo = 0;
	for(currentPageNb=0;currentPageNb<nbPages;currentPageNb++)
	{
		currentFrameNo=findFirstUnoccupied(currentFrameNo);
		if(currentFrameNo==-1)
			return NULL;
		setOccupied(currentFrameNo);
		/*		A COMPLETER !!!		*/
		if(LinkLogAddrToPhyAddr(0,
								(uint32_t*)(currentFrameNo*PAGE_SIZE),
								addrMem+currentPageNb*PAGE_SIZE,
								0,
								0,
								0) 
									== ADD_TT_ERR_LOGICAL_ADDR_EXISTING)
			return NULL;
	}
	return addrMem;	
}

void vMem_Free(uint32_t* ptr, unsigned int nbPages)
{
    /* On raisonne en PAGE, mais on prend une adresse en param */
	uint32_t currentPage;
	for(currentPage=0;currentPage<nbPages;currentPage++)
	{
		/* A completer! */
		uint32_t* phyAddr = GetPhyFromLog(0,ptr+currentPage*PAGE_SIZE);
		/* ATTENTION DIVISION */
		setUnoccupied((uint32_t)phyAddr/PAGE_SIZE);
	}
	VirtualSpace_Release(ptr,nbPages);
}

uint32_t* getPageDescriptor(uint32_t noPage)
{
    /* On raisonne en PAGE */
    return (uint32_t*)noPage+SECON_LVL_TT_START_ADDR;
}

int32_t findFirstUnoccupied(uint32_t noPageDebut)
{
    /* On raisonne en PAGE
        mais il faudrait raisonner en @ pour la perf*/
    uint32_t currPage;
	uint8_t unoccFind = 0;
    for(currPage=noPageDebut;
        currPage<PAGE_COUN;
        currPage++)
	{
        if(!checkOccupation(currPage))
        {
            unoccFind = 1;
            break;
        }
	}
	if(unoccFind)
        return currPage;
	else
		return -1;
}

int32_t findFirstOccupied(uint32_t noPageDebut)
{
    /* On raisonne en PAGE
        mais il faudrait raisonner en @ pour la perf*/
    uint32_t currPage;
	uint8_t occFind = 0;
    for(currPage=noPageDebut;
        currPage<PAGE_COUN;
        currPage++)
	{
        if(checkOccupation(currPage))
        {
            occFind = 1;
            break;
        }
	}
	if(occFind)
        	return currPage;
	else
		return -1;
}

int checkRangeOccupation(uint32_t pageBegin, uint32_t pageEnd)
{
    /* On raisonne en PAGE */
    uint32_t currentPage;
	for(currentPage=pageBegin;currentPage<pageEnd;currentPage++)
		//Il suffit qu'une seule page soit occupee
		//pour faire echouer la procedure
		if(checkOccupation(currentPage))
			return 1;
	
	return 0;
}

int checkOccupation(uint32_t noPage)
{	
    /* On raisonne entre PAGE & ADRESSE */
    uint32_t* occupTableAddr = noPageToPageLineAdress(noPage);
	uint8_t occupTableOffs = noPage%8;
	uint8_t occupModifierLine = 1 << occupTableOffs;
	return ((*occupTableAddr & occupModifierLine));		
}

void setOccupied(uint32_t noPage)
{
    /* On raisonne entre PAGE & ADRESSE */
    uint32_t* occupTableAddr = noPageToPageLineAdress(noPage);
	uint8_t occupTableOffs = noPage%8;
	uint8_t occupModifierLine = 1 << occupTableOffs;
	*occupTableAddr = *occupTableAddr | occupModifierLine;
}

void setUnoccupied(uint32_t noPage)
{
    /* On raisonne entre PAGE & ADRESSE */
    uint32_t* occupTableAddr = noPageToPageLineAdress(noPage);
    uint8_t occupTableOffs = noPage%8;
    uint8_t occupModifierLine = 255
                                & ~(1 << occupTableOffs);
	*occupTableAddr = *occupTableAddr & occupModifierLine;
}

void initFramesTable()
{
    /* On bloque toutes les pages utilisees par le systeme
     * Ici on raisonne en @ & PAGE
    */

    uint32_t currentPage;
	uint32_t initPageFirst;
	uint32_t endPageFirst;
	uint32_t initPageSecon;
	uint32_t endPageSecon;
	Divide(	NO_TRANS_BEG_ADDR1,
			PAGE_SIZE,
			&initPageFirst,&globMod);
	Divide(	NO_TRANS_END_ADDR1,
			PAGE_SIZE,
			&endPageFirst, &globMod);

	Divide(	NO_TRANS_BEG_ADDR2,
			PAGE_SIZE,
			&initPageSecon,&globMod);
	Divide(	NO_TRANS_END_ADDR2,
			PAGE_SIZE,
			&endPageSecon, &globMod);

	for(currentPage=initPageFirst;currentPage<endPageFirst;currentPage++)
    {
        setOccupied(currentPage);
    }

    for(currentPage=initPageSecon;currentPage<initPageSecon;currentPage++)
    {
        setOccupied(currentPage);
    }
}

uint32_t* noPageToPageLineAdress(uint32_t noPage)
{
    uint32_t occupTableLine = noPage/8;
    uint32_t occupTableAddr = (occupTableLine+FRAMES_OCCUP_TABLE_ADDR);
    return (uint32_t*)occupTableAddr;
}





uint32_t* CreateMemoryArea()
/* Algo :
 *	Les tables primaires et secondaires sont allouees
 * dans un espace ou les adresses physiques sont les
 * memes que les adresses logiques
*/
{
	uint32_t* addrPrimaryTable = Mini_Alloc(MINI_SIZE_TO_NB_PAGES(FIRST_LVL_TT_SIZE),14);
    InitFirstEntries(addrPrimaryTable);

    //On cherche une page libre pour la pile
    uint32_t* stackPhysicalAddr = (uint32_t*)(findFirstUnoccupied(0)*PAGE_SIZE);
    LinkLogAddrToPhyAddr(	addrPrimaryTable,
                            stackPhysicalAddr,
                            (uint32_t*)(INIT_STACK_POINTER-PAGE_SIZE+1),
                            PROCES_FIRST_TT_FLAGS,
							PROCES_SECON_TT_FLAGS,
                            0);

	return addrPrimaryTable;
}

uint8_t LinkLogAddrToPhyAddr(	uint32_t* primaryTableAddr,
								uint32_t* physicalAddr,
								uint32_t* desirededLogicalAddr, 
								uint32_t primaryFlags,
								uint32_t secondaryFlags,
								uint8_t rewriteIfExisting)
/* Algo :
 *	On calcule d'abord dans quelle entree de la table primaire
 * puis dans quelle entree de la table secondaire on doit ecrire
 * l'@ physique
 */ 
{
    uint32_t* primaryEntryAddr = (uint32_t*) GET_PRIMARY_ENTRY_ADDR(primaryTableAddr,desirededLogicalAddr);

	if( IS_PRIMARY_TRANS_FAULT(*primaryEntryAddr) )
	{
		*primaryEntryAddr = (uint32_t)Mini_Alloc(MINI_SIZE_TO_NB_PAGES(SECON_LVL_TT_SIZE),10) | (/* ERREUR ? 0xFFFFFC00 |*/ primaryFlags);
	}
	
	uint32_t * secondaryEntryAddr = (uint32_t*) GET_SECONDARY_ENTRY_ADDR(GET_SECONDARY_TABLE_ADDR(*primaryEntryAddr),desirededLogicalAddr);
	
	if(IS_SECONDARY_TRANS_FAULT(secondaryEntryAddr))
	{
		*secondaryEntryAddr = ((uint32_t) physicalAddr | secondaryFlags);
		return ADD_TT_SUCCESS;
	} else {
		if(rewriteIfExisting)
			*secondaryEntryAddr = ((uint32_t) physicalAddr | secondaryFlags);
		return ADD_TT_ERR_LOGICAL_ADDR_EXISTING;
	}
}


/* ---------------------- MINI-TABLE
 *
 *
 *
 * */

int32_t Mini_FindFirstUnoccupied(uint32_t noPageDebut)
{
    /* On raisonne en PAGE
        mais il faudrait raisonner en @ pour la perf*/
    uint32_t currPage;
    uint8_t unoccFind = 0;
    for(currPage=noPageDebut;
        currPage<MINI_FRAMES_FRAMES_COUN;
        currPage++)
    {
        if(!Mini_CheckOccupation(currPage))
        {
            unoccFind = 1;
            break;
        }
    }
    if(unoccFind)
        return currPage;
    else
        return -1;
} //

int32_t Mini_FindFirstOccupied(uint32_t noPageDebut)
{
    /* On raisonne en PAGE
        mais il faudrait raisonner en @ pour la perf*/
    uint32_t currPage;
    uint8_t occFind = 0;
    for(currPage=noPageDebut;
        currPage<MINI_FRAMES_FRAMES_COUN;
        currPage++)
    {
        if(Mini_CheckOccupation(currPage))
        {
            occFind = 1;
            break;
        }
    }
    if(occFind)
            return currPage;
    else
        return -1;
} //

int Mini_CheckRangeOccupation(uint32_t pageBegin, uint32_t pageEnd)
{
    /* On raisonne en PAGE */
    uint32_t currentPage;
    for(currentPage=pageBegin;currentPage<pageEnd;currentPage++)
    {
        //Il suffit qu'une seule page soit occupee
        //pour faire echouer la procedure
        if(Mini_CheckOccupation(currentPage))
        {   return 1;  }
    }
    return 0;
} //

int Mini_CheckOccupation(uint32_t noPage)
{
    /* On raisonne entre PAGE & ADRESSE */
    uint32_t* occupTableAddr = Mini_NoPageToPageLineAdress(noPage);
    uint8_t occupTableOffs = noPage%8;
    uint8_t occupModifierLine = 1 << occupTableOffs;
    return ((*occupTableAddr & occupModifierLine));
} //

void Mini_SetOccupied(uint32_t noPage)
{
    /* On raisonne entre PAGE & ADRESSE */
    uint32_t* occupTableAddr = Mini_NoPageToPageLineAdress(noPage);
    uint8_t occupTableOffs = noPage%8;
    uint8_t occupModifierLine = 1 << occupTableOffs;
    *occupTableAddr = *occupTableAddr | occupModifierLine;
} //

void Mini_SetUnoccupied(uint32_t noPage)
{
    /* On raisonne entre PAGE & ADRESSE */
    uint32_t* occupTableAddr = Mini_NoPageToPageLineAdress(noPage);
    uint8_t occupTableOffs = noPage%8;
    uint8_t occupModifierLine = 255
                                & ~(1 << occupTableOffs);
    *occupTableAddr = *occupTableAddr & occupModifierLine;
} //

void Mini_InitPagesTable()
{
    uint32_t currentPage;
    for(currentPage=0;currentPage<MINI_FRAMES_FRAMES_COUN;currentPage++)
    {
        Mini_SetUnoccupied(currentPage);
    }
} //

uint32_t* Mini_NoPageToPageLineAdress(uint32_t noPage)
{
    uint32_t occupTableLine = noPage/8;
    uint32_t occupTableAddr = (occupTableLine+MINI_FRAMES_ADDR);
    return (uint32_t*)occupTableAddr;
} //


/* --------------- MINI ALLOC, MINI FREE
 */

uint32_t* Mini_Alloc(uint32_t nbMiniFrames, uint8_t nbZeros)
{
    /* On raisonne en PAGE, mais on renvoie une adresse */
    uint32_t addrMem = NULL;
    int32_t noPage = 0;
	uint32_t addrStep = 1 << nbZeros;

    do {
        noPage = Mini_FindFirstUnoccupied(noPage);
        if(noPage==-1)
            //On est arrive au bout de l'espace
            return NULL;

		// Dans le cas où l'on veuille des zeros
		// en fin d'adresse, on corrige le noPage
		if(nbZeros>0)
		{
			// On evite la division psk c'est le bordel
			uint32_t noBloc = 1;
			while(noBloc*addrStep<noPage*MINI_FRAMES_SIZE_OF_A_FRAME)
			{
				noBloc++;
			}
			uint32_t noPageU;
			Divide(	noBloc*addrStep,
					MINI_FRAMES_SIZE_OF_A_FRAME,
					&noPageU, &globMod);
			noPage = (int32_t)noPageU;
		}

        if(!Mini_CheckRangeOccupation(noPage,noPage+nbMiniFrames))
        {
            break;
        }
        noPage = Mini_FindFirstOccupied((uint32_t) noPage);
    } while(noPage!=-1);

    if(noPage==-1)
        return NULL;
    //On a notre plage de pages
    //On charge en retour l'adresse initiale
	addrMem = noPage*MINI_FRAMES_SIZE_OF_A_FRAME+KERNEL_MRY_ADDR;

    //Remplissage de la table des pages
    uint32_t noCurrentPage;
    for(noCurrentPage=noPage;
        noCurrentPage<noPage+nbMiniFrames;
        noCurrentPage++)
    {
        Mini_SetOccupied(noCurrentPage);
    }
    return (uint32_t*)addrMem;
}

uint8_t Mini_Free(uint32_t* ptTable, uint32_t nbMiniFrames)
{
    uint32_t currentPage;
	uint32_t pageInit;
	uint32_t pageEnd;
	uint32_t mod;	//pour divide, mais inutile
	Divide(((uint32_t) ptTable-KERNEL_MRY_ADDR),
			MINI_FRAMES_SIZE_OF_A_FRAME,
			&pageInit,&mod);

	Divide(((uint32_t) ptTable-KERNEL_MRY_ADDR),
			MINI_FRAMES_SIZE_OF_A_FRAME+nbMiniFrames,
			&pageEnd,&mod);

	for(currentPage = pageInit;
		currentPage < pageEnd;
        currentPage++)
    {
        Mini_SetUnoccupied(currentPage);
    }

	return 0;
}

/* ------------------ VIRTUAL SPACE ALLOCATOR
 */

FreeSpace* VirtualSpace_Find(uint32_t nbPages)
{
	FreeSpace* currentFreeSpace = ptFirstFreeSpace; 
	do {
		if((*currentFreeSpace).nbPagesFree > nbPages)
			return currentFreeSpace;
		else 
			currentFreeSpace = (*currentFreeSpace).ptNextFreeSpace;
	} while(currentFreeSpace!=ptFirstFreeSpace);
	return NULL;
}

uint32_t* VirtualSpace_Get(uint32_t nbPages)
{
	FreeSpace* freeSpace = VirtualSpace_Find(nbPages);
	if(freeSpace==NULL)
		return NULL;
	uint32_t* addr = (*freeSpace).addrSpace;
	VirtualSpace_Fill(freeSpace,nbPages);
	return addr;
}

void VirtualSpace_Fill(FreeSpace* freeSpace, uint32_t nbPages)
/* ALGO :
 * deux cas:si l'espace fait pile la bonne taille, alors on
 *				supprime cet espace (qui n'est plus libre)
 *				puis on linke l'espace libre precedent directement
 *				avec le suivant
 *			si l'espace est plus petit, on reserve la partie
 *				inferieure en decalant l'adresse de l'espace libre
 *				et en enlevant le nb de pages
 */
{
	if(nbPages >= (*freeSpace).nbPagesFree)
	{
		(*VirtualSpace_GetPrevious(freeSpace)).ptNextFreeSpace = (*freeSpace).ptNextFreeSpace;
		if(freeSpace==ptFirstFreeSpace)
		{
			ptFirstFreeSpace = (*freeSpace).ptNextFreeSpace;
		}

		Mini_Free(	(uint32_t*)freeSpace,
					MINI_SIZE_TO_NB_PAGES(sizeof(FreeSpace)) );
	} else {
		(*freeSpace).addrSpace += nbPages*PAGE_SIZE;
		(*freeSpace).nbPagesFree -= nbPages;
	}
}

void VirtualSpace_Release(uint32_t* logAddrToRealease, uint32_t nbPages)
{
	FreeSpace* nextFS = VirtualSpace_GetNextFreeSpace(logAddrToRealease);
	FreeSpace* prevFS = VirtualSpace_GetPrevious(nextFS);
	uint8_t createNewFS = 1;
	if((*nextFS).addrSpace==logAddrToRealease+nbPages*PAGE_SIZE)
	{
		(*nextFS).addrSpace = logAddrToRealease;
		(*nextFS).nbPagesFree += nbPages;

		createNewFS = 0;
	}
	
	if((*prevFS).addrSpace+(*prevFS).nbPagesFree*PAGE_SIZE==logAddrToRealease)
	{
		(*prevFS).nbPagesFree += nbPages;

		createNewFS = 0;
	}

	if(createNewFS)
	{		
		uint32_t nbMiniFrames;
		uint32_t mod;
		Divide(	sizeof(FreeSpace),
				MINI_FRAMES_SIZE_OF_A_FRAME+1,
				&nbMiniFrames,&mod);
		FreeSpace* newFS = (FreeSpace*)Mini_Alloc(nbMiniFrames,0);
		(*newFS).nbPagesFree = nbPages;
		(*newFS).addrSpace = logAddrToRealease;
		(*newFS).ptNextFreeSpace = nextFS;
		(*prevFS).ptNextFreeSpace = newFS;
	}

}

FreeSpace* VirtualSpace_GetPrevious(FreeSpace* freeSpace)
{
	FreeSpace* currentSpace = ptFirstFreeSpace;
	while((*currentSpace).ptNextFreeSpace!=freeSpace)
	{
		currentSpace = (*currentSpace).ptNextFreeSpace;
	}
	return currentSpace;
}

FreeSpace* VirtualSpace_GetNextFreeSpace(uint32_t* logAddrToRealease)
{
	FreeSpace* currentFS = ptFirstFreeSpace;
	uint32_t* previousAddr = 0;
	while((*currentFS).addrSpace<logAddrToRealease 
			&& previousAddr<=(*currentFS).addrSpace)
	{
		previousAddr = (*currentFS).addrSpace+1;
		currentFS = (*currentFS).ptNextFreeSpace;
	}
	return currentFS;
}

void vMem_Init(FreeSpace* pt_firstFSDescriptor)
{
	uint32_t nbPagesFree;
	uint32_t mod;
	Divide(	(HEAP_END-HEAP_BEG),
			PAGE_SIZE,
			&nbPagesFree,&mod);
	
	(*pt_firstFSDescriptor).nbPagesFree = nbPagesFree;
	(*pt_firstFSDescriptor).addrSpace = (uint32_t*)HEAP_BEG;
	(*pt_firstFSDescriptor).ptNextFreeSpace = ptFirstFreeSpace;
}

uint32_t* Kernel_InitTTEntries()
/* Initialisation de la non-translation log->phy
 * pour les adresses du noyau et du materiel
 * = creation de la Translation Table du Noyau
 */
{
    uint32_t primaryTable_flag = KERNEL_FIRST_TT_FLAGS;
    uint32_t primaryTable_transErrorFlag = PRIMARY_TRANS_FAULT_1;
    uint32_t device_flag = KERNEL_SECON_TT_FLAGS_DEVICES;
    uint32_t normal_flag = KERNEL_SECON_TT_FLAGS_INIT;
	uint32_t* currentAddr;
	uint32_t* addrKernelTT = Mini_Alloc(MINI_SIZE_TO_NB_PAGES(FIRST_LVL_TT_SIZE),14);

	for(currentAddr=(uint32_t*)NO_TRANS_BEG_ADDR1;
		currentAddr<(uint32_t*)NO_TRANS_END_ADDR1;
		currentAddr+=PAGE_SIZE/4)
	{
		LinkLogAddrToPhyAddr(addrKernelTT,currentAddr,currentAddr,primaryTable_flag,normal_flag,1);
	}

	for(currentAddr=(uint32_t*)NO_TRANS_BEG_ADDR2;
		currentAddr<(uint32_t*)NO_TRANS_END_ADDR2;
		currentAddr+=PAGE_SIZE/4)
	{
		LinkLogAddrToPhyAddr(addrKernelTT,currentAddr,currentAddr,primaryTable_flag,device_flag,1);
	}
	return addrKernelTT;
}


void InitFirstEntries(uint32_t* primaryTableAddr)
/* On copie les premieres entrees de la table primaire du Noyau
 * a la place des premieres entrees de la table primaire du PS
 */
{
	uint32_t* currentLogicalAddress;
	uint32_t* addrEntryInKerTT;
	uint32_t* addrEntryInCurTT;
	
	for(currentLogicalAddress=(uint32_t*)NO_TRANS_BEG_ADDR1;
		currentLogicalAddress<(uint32_t*)NO_TRANS_END_ADDR1;
		currentLogicalAddress+=(PAGE_SIZE*SECON_LVL_TT_COUN)/4 )
	{
		addrEntryInKerTT = (uint32_t*)GET_PRIMARY_ENTRY_ADDR(Kernel_FirstTTAddr,currentLogicalAddress);
		addrEntryInCurTT = (uint32_t*)GET_PRIMARY_ENTRY_ADDR(primaryTableAddr,currentLogicalAddress);

		PUT32(	(uint32_t)addrEntryInCurTT,
				*addrEntryInKerTT );
	}
	for(currentLogicalAddress=(uint32_t*)NO_TRANS_BEG_ADDR2;
		currentLogicalAddress<(uint32_t*)NO_TRANS_END_ADDR2;
		currentLogicalAddress+=(PAGE_SIZE*SECON_LVL_TT_COUN)/4 )
	{
		addrEntryInKerTT = (uint32_t*)GET_PRIMARY_ENTRY_ADDR(Kernel_FirstTTAddr,currentLogicalAddress);
		addrEntryInCurTT = (uint32_t*)GET_PRIMARY_ENTRY_ADDR(primaryTableAddr,currentLogicalAddress);

		PUT32(	(uint32_t)addrEntryInCurTT,
				*addrEntryInKerTT );
	}
}


uint32_t* GetPhyFromLog(uint32_t* primaryTableAddr, uint32_t* logAddr)
{
	uint32_t* primaryTableLineAddr = (uint32_t*)GET_PRIMARY_ENTRY_ADDR(primaryTableAddr,logAddr);
	uint32_t* secTableLineAddr =  (uint32_t*)GET_SECONDARY_ENTRY_ADDR(GET_SECONDARY_TABLE_ADDR(*primaryTableLineAddr),logAddr);
	uint32_t* phyAddr = (uint32_t*)((uint32_t)((*secTableLineAddr) & 0xFFFFF000) + ((uint32_t)(logAddr) & 0x00000FFF));  
	return phyAddr;
}



/* ------------- Utilitaires
 */

uint8_t Divide(uint32_t top, uint32_t btm, uint32_t* ptRslt, uint32_t* ptMod)
{
	uint32_t incr;
	uint32_t somme = btm;
	if(btm==0)
		return DVD_ZERO_ERROR;

	for(incr=0;somme<=top;incr++)
	{
		somme += btm;
	}
	*ptRslt = incr;
	*ptMod = top+btm-somme;
	return DVD_SUCCES;
}

