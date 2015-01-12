#include "vmem.h"
#include "../utilities/utilities.h"
#include "Mini_Alloc.h"
#include "Frames.h"
#include <stdio.h>

/* VARIABLES GLOBALES */

//Variables pot de fleur
unsigned int total=0;	//Pour start MMU
uint32_t globMod=0;		//Modulo global (pour la division)
FreeSpace* ptFirstFreeSpace;

//MMU 
uint32_t* Kernel_FirstTTAddr;
extern uint8_t Kernel_ASID;


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





// Enough shit, here we have the --------------
// --------------USEFUL FONCTIONS--------------
// --------------------------------------------


/* CONFIGURATION FONCTIONS 
 * MMU configuration fonctions
 *
*/
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
	uint32_t stackPhyPage = findFirstUnoccupied(0);
	setOccupied(stackPhyPage);
	uint32_t* stackPhysicalAddr = (uint32_t*)(stackPhyPage*PAGE_SIZE);
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



uint32_t* Kernel_InitTTEntries()
/* Initialisation de la non-translation log->phy
 * pour les adresses du noyau et du materiel
 * = creation de la Translation Table du Noyau
 */
{
    uint32_t primaryTable_flag = KERNEL_FIRST_TT_FLAGS;
    uint32_t device_flag = KERNEL_SECON_TT_FLAGS_DEVICES;
    uint32_t normal_flag = KERNEL_SECON_TT_FLAGS_INIT;
	uint32_t* currentAddr;
	uint32_t* addrKernelTT = Mini_Alloc(MINI_SIZE_TO_NB_PAGES(FIRST_LVL_TT_SIZE),14);

	for(currentAddr=(uint32_t*)NO_TRANS_BEG_ADDR1;
		currentAddr<(uint32_t*)NO_TRANS_END_ADDR1;
		currentAddr+=PAGE_SIZE/4)
	{
		if(currentAddr>=(uint32_t*)KERNEL_FREE_AREA_BEG && currentAddr<(uint32_t*)KERNEL_FREE_AREA_END)
			LinkLogAddrToPhyAddr(addrKernelTT,currentAddr,currentAddr,primaryTable_flag,KERNEL_SECON_TT_FLAGS_FREE,1);
		else
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




