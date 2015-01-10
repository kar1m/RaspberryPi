#include "VMem_Constants.h"
#include "Frames.h"
#include "../utilities/utilities.h"

uint32_t globMod;

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
