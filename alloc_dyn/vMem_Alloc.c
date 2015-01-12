#include <stddef.h>
#include "vMem_Alloc.h"
#include "../VirtualMemory/Frames.h"
#include "../VirtualMemory/vmem.h"
#include "../utilities/utilities.h"
#include "../VirtualMemory/Mini_Alloc.h"

/* ---------------- ALLOC/FREE --------------------
*/
extern FreeSpace* ptFirstFreeSpace;

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

void vMem_Init(FreeSpace* pt_firstFSDescriptor)
{
	uint32_t nbPagesFree;
	uint32_t mod;
	Divide(	(HEAP_END-HEAP_BEG),
			PAGE_SIZE,
			&nbPagesFree,&mod);
	
	(*pt_firstFSDescriptor).nbPagesFree = nbPagesFree;
	(*pt_firstFSDescriptor).addrSpace = (uint32_t*)HEAP_BEG;
	(*pt_firstFSDescriptor).ptNextFreeSpace = pt_firstFSDescriptor;
}

/* ---------------- VIRTUAL SPACE -----------------
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


