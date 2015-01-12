#include "Mini_Alloc.h"
#include "../utilities/utilities.h"
#include <stdio.h>

uint32_t globMod;
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
    uint32_t addrMem = (uint32_t)NULL;
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
