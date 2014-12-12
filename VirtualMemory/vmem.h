#ifndef VMEM_H
#define VMEM_H

#include <stdint.h>


//Definition des constantes de pagination
#define PAGE_SIZE 4096
#define SECON_LVL_TT_COUN 256
#define SECON_LVL_TT_SIZE 1024
#define FIRST_LVL_TT_COUN 4096 
#define FIRST_LVL_TT_SIZE 16384
#define SECTION_SIZE (SECON_LVL_TT_COUN * PAGE_SIZE)
#define TOTAL_TT_SIZE (FIRST_LVL_TT_SIZE + SECON_LVL_TT_SIZE * FIRST_LVL_TT_COUN)
#define PAGE_COUN (0x21000000/PAGE_SIZE)

//Adresses des tables
#define FIRST_LVL_TT_START_ADDR 0x48000
#define SECON_LVL_TT_START_ADDR (FIRST_LVL_TT_SIZE + FIRST_LVL_TT_START_ADDR)
#define SECON_LVL_TT_END_ADDR (SECON_LVL_TT_START_ADDR + SECON_LVL_TT_SIZE * FIRST_LVL_TT_COUN)
#define MMUTABLEBASE (FIRST_LVL_TT_START_ADDR)

//Adresses limites de translation
#define NO_TRANS_BEG_ADDR1 0x0
#define NO_TRANS_END_ADDR1 0x700000
#define NO_TRANS_BEG_ADDR2 0x20000000
#define NO_TRANS_END_ADDR2 0x21000000

//Table d'occupation des frames
#define FRAMES_OCCUP_TABLE_ADDR 0x4FBE00
#define FRAMES_OCCUP_TABLE_SIZE 0x4200

unsigned int init_kern_translation_table();
void generateTestValues();
int testVM();
void start_mmu_C(); 
void configure_mmu_C();
uint8_t* vMem_Alloc(unsigned int nbPages);
void vMem_Free(uint8_t* ptr, unsigned int nbPages);
extern void PUT32( unsigned int, unsigned int);
void setOccupied(uint32_t noPage);
void setUnoccupied(uint32_t noPage);
int32_t findFirstUnoccupied(uint32_t noPageDebutRecherche);
uint32_t* getPageDescriptor(uint32_t noPage);
int checkRangeOccupation(uint32_t noPageDebut, uint32_t noPageFin);
int checkOccupation(uint32_t noPage);
void initPagesTable();
uint32_t* noPageToPageLineAdress(uint32_t noPage);


#endif
