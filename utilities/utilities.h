#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdint.h>

/* -------------- UTILITAIRES
 *
 */
#define DVD_ZERO_ERROR 0
#define DVD_SUCCES 1
uint8_t Divide(uint32_t top, uint32_t btm, uint32_t* ptRslt, uint32_t* ptMod);
/* Fonction permettant de diviser avec le RaspberryPi, qui semble-t'il, n'est
 * pas tres a l'aise avec ca
 */

extern void PUT32( unsigned int addr, unsigned int content);
/* Fonction permettant d'ajouter manuellement 32 bits en
 * memoire
 */

#endif