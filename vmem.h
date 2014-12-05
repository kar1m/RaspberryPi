#ifndef VMEM_H
#define VMEM_H

#include <stdint.h>

//Definition des constantes de pagination
#define PAGE_SIZE 4096
#define SECON_LVL_TT_COUN 256
#define SECON_LVL_TT_SIZE 1024
#define FIRST_LVL_TT_COUN 4096 
#define FIRST_LVL_TT_SIZE 16384
#define TOTAL_TT_SIZE (FIRST_LVL_TT_SIZE + SECON_LVL_TT_SIZE*FIRST_LVL_TT_COUN)

//Adresses des tables
#define FIRST_LVL_TT_START_ADDR 0x48000
#define SECON_LVL_TT_START_ADDR 0x49000
#define SECON_LVL_TT_END_ADDR 0x148FFF

//Adresses limites de translation
#define NO_TRANS_BEG_ADDR1 0x0
#define NO_TRANS_END_ADDR1 0x500000
#define NO_TRANS_BEG_ADDR2 0x20000000
#define NO_TRANS_END_ADDR2 0x21000000

unsigned int init_kern_translation_table();



#endif
