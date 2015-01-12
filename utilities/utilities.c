#include "utilities.h"

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
