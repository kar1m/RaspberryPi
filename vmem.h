#ifndef VMEM_H
#define VMEM_H


//Definition des constantes de pagination
#define PAGE_SIZE 4096
#define SECON_LVL_TT_COUN 256
#define SECON_LVL_TT_SIZE 1048576
#define FIRST_LVL_TT_COUN 4096
#define FIRST_LVL_TT_SIZE 4096
#define TOTAL_TT_SIZE FIRST_LVL_TT_SIZE + SECON_LVL_TT_SIZE

#define FIRST_LVL_TT_START_ADDR 0x48000
#define SECON_LVL_TT_START_ADDR 0x49000
#define SECON_LVL_TT_END_ADDR 0x148FFF

unsigned int init_kern_translation_table();



#endif
