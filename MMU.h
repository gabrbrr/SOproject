#pragma once
#include <stdint.h>
#define PAGE_NUM_BIT 12
#define MEM_NUM_BIT 20
#define VIRTUAL_NUM_BIT 24
#define PAGE_SIZE 1<<PAGE_NUM_BIT
#define PHYSICAL_MEM_SIZE 1<< MEM_NUM_BIT
#define VIRTUAL_MEM_SIZE 1<< VIRTUAL_NUM_BIT
#define SWAP_FILE_SIZE 1<<VIRTUAL_NUM_BIT

typedef struct PageTableEntry{
    unsigned int frame : MEM_NUM_BIT;
    unsigned int valid :1;
    unsigned int unswappable : 1;
    unsigned int read_bit : 1;
    unsigned int write_bit : 1;
    unsigned int reference_bit :1;
} PageTableEntry;

//1048576   4096*4 = 16384   

typedef struct MMU{
    char * physical_memory;
    char * swap_file;
    char * buffer;
    PageTableEntry * page_table;
    uint16_t * frame_to_page;
    int num_frames;
} MMU;


void MMU_init(MMU* mmu,char *phy,char * swap);
void MMU_exception(MMU* mmu, int pos);
void MMU_writeByte(MMU* mmu, int pos, char c);
char* MMU_readByte(MMU* mmu, int pos);