#ifndef VCONST_H
#define VCONST_H

// PAGES
#define PAGE_SIZE 4096

// TRANSLATION TABLE
#define SECON_LVL_TT_COUN 256
#define SECON_LVL_TT_SIZE 1024
#define FIRST_LVL_TT_COUN 4096 
#define FIRST_LVL_TT_SIZE 16384
#define SECTION_SIZE (SECON_LVL_TT_COUN * PAGE_SIZE)
#define TOTAL_TT_SIZE (FIRST_LVL_TT_SIZE + SECON_LVL_TT_SIZE * FIRST_LVL_TT_COUN) //0x404000=4.210.688
#define PAGE_COUN (0x21000000/PAGE_SIZE)

// MINI
#define MINI_FRAMES_SIZE_OF_A_FRAME 16
#define MINI_FRAMES_TOTAL_SIZE (KERNEL_MRY_SIZE/MINI_FRAMES_SIZE_OF_A_FRAME/8)
#define MINI_FRAMES_FRAMES_COUN MINI_FRAMES_TOTAL_SIZE*8

// TABLE FRAMES
#define FRAMES_OCCUP_TABLE_SIZE (0x21000000/PAGE_SIZE) //16896=0x4200

//  Adresses des tables (TABLES KERNEL)
// --------------DEPRECATED--------------
//  Utilisation systematique de Mini_Alloc
// pour l'attribution des tables primaires
// et secondaires
#define FIRST_LVL_TT_START_ADDR 0x48000
#define SECON_LVL_TT_START_ADDR (FIRST_LVL_TT_SIZE + FIRST_LVL_TT_START_ADDR)
#define SECON_LVL_TT_END_ADDR (SECON_LVL_TT_START_ADDR + SECON_LVL_TT_SIZE * FIRST_LVL_TT_COUN) 
#define MMUTABLEBASE (FIRST_LVL_TT_START_ADDR)

//  Zone memoire Kernel
// --------------DEPRECATED--------------
#define KERNEL_MRY_CMPL 0xC000
#define KERNEL_MRY_SIZE KERNEL_MRY_CMPL+TOTAL_TT_SIZE

// @MEMOIRE
#define KERNEL_MRY_ADDR 0x48000
#define MINI_FRAMES_ADDR (KERNEL_MRY_ADDR+KERNEL_MRY_SIZE)
#define FRAMES_OCCUP_TABLE_ADDR (MINI_FRAMES_ADDR+MINI_FRAMES_TOTAL_SIZE)


//Adresses limites de translation
#define NO_TRANS_BEG_ADDR1 0x0
// ------NON UTILISE POUR L'INSTANT------
#define NO_TRANS_END_ADDR1 (FRAMES_OCCUP_TABLE_SIZE+FRAMES_OCCUP_TABLE_ADDR)
// -> Forc� a 0x500 000
#define NO_TRANS_END_ADDR1 0x500000
#define NO_TRANS_BEG_ADDR2 0x20000000
#define NO_TRANS_END_ADDR2 0x21000000
#define KERNEL_FREE_AREA_BEG 0x8000
#define KERNEL_FREE_AREA_END 0x40000

#endif