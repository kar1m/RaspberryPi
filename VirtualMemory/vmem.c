#include "vmem.h"
#include <stdio.h>

unsigned int total=0;

unsigned int init_kern_translation_table()
{
/* Initialisation de la table de
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
	uint8_t ret = checkOccupation(0x600);
	setOccupied(0x600);
	uint8_t ret1 = checkOccupation(0x600);
	setUnoccupied(0x600);
	uint8_t ret2 = checkOccupation(0x600);
    uint32_t * area1 = vMem_Alloc(1);
    uint32_t * area2 = vMem_Alloc(2);
    vMem_Free(area2,2);
    uint32_t * area3 = vMem_Alloc(2);
    vMem_Free(area1,1);
	uint32_t * area4 = vMem_Alloc(4);
	uint32_t * area5 = vMem_Alloc(1);
    return 0;
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
}

void configure_mmu_C()
{
	register unsigned int pt_addr = MMUTABLEBASE;
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
    uint32_t addrMem = NULL;
	
    /*do {
		noPage = findFirstUnoccupied(noPage);
		if(noPage==-1)
			//On est arrive au bout de l'espace
			return NULL;
		if(!checkRangeOccupation(noPage,noPage+nbPages))
		{
			break;
		}
		noPage = findFirstOccupied((uint32_t) noPage);
	} while(noPage!=-1);
	
	if(noPage==-1)
		return NULL;
	//On a notre plage de pages
	//On charge en retour l'adresse initiale
	addrMem = noPage*PAGE_SIZE;
	
	//Remplissage de la table des pages
	uint32_t noCurrentPage;
    for(noCurrentPage=noPage;
		noCurrentPage<noPage+nbPages;
		noCurrentPage++)
	{
		setOccupied(noCurrentPage);
    }*/
	return addrMem;	
}

void vMem_Free(uint32_t* ptr, unsigned int nbPages)
{
    /* On raisonne en PAGE, mais on prend une adresse en param */
    uint32_t currentPage;
    for(currentPage = (uint32_t) ptr/PAGE_SIZE;currentPage<(uint32_t) ptr/PAGE_SIZE+nbPages;currentPage++)
    {
        setUnoccupied(currentPage);
    }
	
}

uint32_t* getPageDescriptor(uint32_t noPage)
{
    /* On raisonne en PAGE */
    return noPage+SECON_LVL_TT_START_ADDR;
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

void initPagesTable()
{
    /* On bloque toutes les pages utilisees par le systeme
     * Ici on raisonne en @ & PAGE
    */

    uint32_t currentPage;
    for(currentPage=NO_TRANS_BEG_ADDR1/PAGE_SIZE;currentPage<(NO_TRANS_END_ADDR1-0x100000)/PAGE_SIZE;currentPage++)
    {
        setOccupied(currentPage);
    }

    for(currentPage=NO_TRANS_BEG_ADDR2/PAGE_SIZE;currentPage<NO_TRANS_END_ADDR2/PAGE_SIZE;currentPage++)
    {
        setOccupied(currentPage);
    }
}

uint32_t* noPageToPageLineAdress(uint32_t noPage)
{
    uint32_t occupTableLine = noPage/8;
    uint32_t occupTableAddr = (occupTableLine+FRAMES_OCCUP_TABLE_ADDR);
    return occupTableAddr;
}





uint32_t* CreateMemoryArea()
/* Algo :
 *	Les tables primaires et secondaires sont allouees
 * dans un espace ou les adresses physiques sont les
 * memes que les adresses logiques
*/
{
    uint32_t* addrPrimaryTable = MiniAlloc(FRAMES_TT_FIRST_TABLE_OCCUP);
    InitFirstEntries(addrPrimaryTable);

    //On cherche une page libre pour la pile
    uint32_t* stackPhysicalAddr = (uint32_t*)findFirstUnoccupied(0)*PAGE_SIZE;
    InsertEntryInSecondaryTable(addrPrimaryTable,
                                stackPhysicalAddr,
                                INIT_STACK_POINTER-PAGE_SIZE+1,
                                0x0,
                                0);

	return addrPrimaryTable;
}

uint8_t InsertEntryInSecondaryTable(uint32_t* primaryTableAddr,
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
    uint32_t* primaryEntryAddr = GET_PRIMARY_ENTRY_ADDR(primaryTableAddr,desirededLogicalAddr);

	if( IS_PRIMARY_TRANSLATION_FAULT(*primaryEntryAddr) )
	{
        *primaryEntryAddr = ((uint32_t)MiniAlloc(FRAMES_TT_SECON_TABLE_OCCUP) & (0xFFFFFC00 | primaryFlags));
	}
	
	uint32_t * secondaryEntryAddr = (uint32_t*) GET_SECONDARY_ENTRY_ADDR(GET_SECONDARY_TABLE_ADDR(*primaryEntryAddr),desirededLogicalAddr);
	
	if(IS_SECONDARY_TRANSLATION_FAULT(secondaryEntryAddr))
	{
		*secondaryEntryAddr = ((uint32_t) physicalAddr) & (0xFFFFF000 | secondaryFlags);
		return ADD_TT_SUCCESS;
	} else {
		if(rewriteIfExisting)
			*secondaryEntryAddr = ((uint32_t) physicalAddr) & (0xFFFFF000 | secondaryFlags);
		return ADD_TT_ERR_LOGICAL_ADDR_EXISTING;
	}
}


/* ---------------------- MINI-TABLE
 *
 *
 *
 * */

int32_t MiniFindFirstUnoccupied(uint32_t noPageDebut)
{
    /* On raisonne en PAGE
        mais il faudrait raisonner en @ pour la perf*/
    uint32_t currPage;
    uint8_t unoccFind = 0;
    for(currPage=noPageDebut;
        currPage<FRAMES_TT_FRAMES_COUN;
        currPage++)
    {
        if(!MiniCheckOccupation(currPage))
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

int32_t MiniFindFirstOccupied(uint32_t noPageDebut)
{
    /* On raisonne en PAGE
        mais il faudrait raisonner en @ pour la perf*/
    uint32_t currPage;
    uint8_t occFind = 0;
    for(currPage=noPageDebut;
        currPage<FRAMES_TT_FRAMES_COUN;
        currPage++)
    {
        if(MiniCheckOccupation(currPage))
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

int MiniCheckRangeOccupation(uint32_t pageBegin, uint32_t pageEnd)
{
    /* On raisonne en PAGE */
    uint32_t currentPage;
    for(currentPage=pageBegin;currentPage<pageEnd;currentPage++)
    {
        //Il suffit qu'une seule page soit occupee
        //pour faire echouer la procedure
        if(MiniCheckOccupation(currentPage))
        {   return 1;  }
    }
    return 0;
} //

int MiniCheckOccupation(uint32_t noPage)
{
    /* On raisonne entre PAGE & ADRESSE */
    uint32_t* occupTableAddr = MiniNoPageToPageLineAdress(noPage);
    uint8_t occupTableOffs = noPage%8;
    uint8_t occupModifierLine = 1 << occupTableOffs;
    return ((*occupTableAddr & occupModifierLine));
} //

void MiniSetOccupied(uint32_t noPage)
{
    /* On raisonne entre PAGE & ADRESSE */
    uint32_t* occupTableAddr = MiniNoPageToPageLineAdress(noPage);
    uint8_t occupTableOffs = noPage%8;
    uint8_t occupModifierLine = 1 << occupTableOffs;
    *occupTableAddr = *occupTableAddr | occupModifierLine;
} //

void MiniSetUnoccupied(uint32_t noPage)
{
    /* On raisonne entre PAGE & ADRESSE */
    uint32_t* occupTableAddr = MiniNoPageToPageLineAdress(noPage);
    uint8_t occupTableOffs = noPage%8;
    uint8_t occupModifierLine = 255
                                & ~(1 << occupTableOffs);
    *occupTableAddr = *occupTableAddr & occupModifierLine;
} //

void MiniInitPagesTable()
{
    uint32_t currentPage;
    for(currentPage=0;currentPage<FRAMES_TT_FRAMES_COUN;currentPage++)
    {
        MiniSetUnoccupied(currentPage);
    }
} //

uint32_t* MiniNoPageToPageLineAdress(uint32_t noPage)
{
    uint32_t occupTableLine = noPage/8;
    uint32_t occupTableAddr = (occupTableLine+FRAMES_TT_ADDR);
    return occupTableAddr;
} //


/* --------------- MINI ALLOC, MINI FREE
 */

uint32_t* MiniAlloc(uint8_t nbMiniFrames)
{
    /* On raisonne en PAGE, mais on renvoie une adresse */
    uint32_t addrMem = NULL;
    int32_t noPage = 0;
    uint32_t nextPage;

    do {
        noPage = MiniFindFirstUnoccupied(noPage);
        if(noPage==-1)
            //On est arrive au bout de l'espace
            return NULL;
        if(!MiniCheckRangeOccupation(noPage,noPage+nbMiniFrames))
        {
            break;
        }
        noPage = MiniFindFirstOccupied((uint32_t) noPage);
    } while(noPage!=-1);

    if(noPage==-1)
        return NULL;
    //On a notre plage de pages
    //On charge en retour l'adresse initiale
    addrMem = noPage*FRAMES_TT_SIZE_OF_A_FRAME+FRAMES_TT_ADDR;

    //Remplissage de la table des pages
    uint32_t noCurrentPage;
    for(noCurrentPage=noPage;
        noCurrentPage<noPage+nbMiniFrames;
        noCurrentPage++)
    {
        MiniSetOccupied(noCurrentPage);
    }
    return addrMem;
}

uint8_t MiniFree(uint32_t* ptTable, uint8_t nbMiniFrames)
{
    uint32_t currentPage;
    for(currentPage = ((uint32_t) ptTable-FRAMES_TT_ADDR)/FRAMES_TT_SIZE_OF_A_FRAME;
        currentPage < ((uint32_t) ptTable-FRAMES_TT_ADDR)/FRAMES_TT_SIZE_OF_A_FRAME+nbMiniFrames;
        currentPage++)
    {
        MiniSetUnoccupied(currentPage);
    }
}

uint32_t* FindFreeSpace(uint32_t nbPages)
{

}


/*ATTENTION ! POUR L'INSTANT LES TRADUCTIONS VERS LES @PERIPHERIQUES
 * SONT DESACTIVEES (TRANSLATION FAULT)
 */

void InitFirstEntries(uint32_t* primaryTableAddr)
{
    uint32_t primaryTable_flag = 0x1;
    uint32_t primaryTable_transErrorFlag = 0x3;
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
            // sont inaccessibles au PS
            primaryTableEntry=primaryTable_transErrorFlag;

        } else if(currentVirtualAddr >= NO_TRANS_BEG_ADDR2 &&
                  currentVirtualAddr < NO_TRANS_END_ADDR2) {
            //Dans cette zone les adresses physiques
            // sont accessibles au PS, sans traduction
            primaryTableEntry=primaryTable_transErrorFlag;
        }
        PUT32(addr, primaryTableEntry);
    }
}
