#include "vmem.h"


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
	for(uint32_t addr=FIRST_LVL_TT_START_ADDR; addr<FIRST_LVL_TT_START_ADDR+FIRST_LVL_TT_SIZE; addr+=4)
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
			currentSecondaryTable_beg_addr += SECON_LVL_TT_SIZE 
		} else {
			tableEntry=primaryTable_transErrorFlag;
		}
		PUT32(addr, tableEntry);
	}
	return 0;
}
