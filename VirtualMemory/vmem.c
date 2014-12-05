#include "vmem.h"

unsigned int total=0;

unsigned int init_kern_translation_table()
{
/* Deconnexion de la MMU
 */

/* Initialisation de la table de
 * niveau 1, à partir de l'adresse
 * specifiee
 */
	uint32_t primaryTable_flag = 0x1;
	uint32_t primaryTable_transErrorFlag = 0x0;
	uint32_t device_flag = 0x2000047E;

	//Adresse de la table de second niveau actuelle
	uint32_t currentSecondaryTable_beg_addr = SECON_LVL_TT_START_ADDR;
	uint32_t addr;
	for(addr=FIRST_LVL_TT_START_ADDR; addr<FIRST_LVL_TT_START_ADDR+FIRST_LVL_TT_SIZE; addr+=4)
	{			
		//Translation d'adresse
		//On determine la case de
		//la table de premier niveau
		uint32_t currentVirtualAddr = addr<<20;
		uint32_t tableEntry;
		if(	(currentVirtualAddr >= NO_TRANS_BEG_ADDR1 &&
			currentVirtualAddr < NO_TRANS_END_ADDR1) ||
			(currentVirtualAddr >= NO_TRANS_BEG_ADDR2 &&
			currentVirtualAddr < NO_TRANS_END_ADDR2) )
		{
			//Dans cette zone les adresses physiques
			// correspondent aux adresses logiques

			int32_t addrInSecTable;
			for(addrInSecTable = 0;
				addrInSecTable<currentSecondaryTable_beg_addr+SECON_LVL_TT_SIZE;
				addrInSecTable+=4)
			{
				uint32_t secTableEntry = currentVirtualAddr^device_flag;
				PUT32(addrInSecTable,secTableEntry);
			}
			//On met a jour l'adresse de la table de second niveau :
			//comme on vient de remplir la table precedente, on pousse
			//l'adresse de la taille d'une table
			currentSecondaryTable_beg_addr += SECON_LVL_TT_SIZE; 
		} else {
			tableEntry=primaryTable_transErrorFlag;
		}
		PUT32(addr, tableEntry);
	}
	return 0;
}

void generateTestValues()
{
	PUT32(0x4FFFFC, 12344321);
	PUT32(0x2000000, 20121992);
}

int testVM()
{
	int result = 0; 
	int* addrResult = 0x4FFFFC;
	if(*addrResult==1234321)
		result+=1;

	addrResult = 0x20000000;
	if(*addrResult==20121992)
		result+=10;
	return result;
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
