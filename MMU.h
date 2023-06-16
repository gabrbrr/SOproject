#pragma once
#include <stdint.h>
#define PAGE_NUM_BIT 12
#define MEM_NUM_BIT 20
#define VIRTUAL_NUM_BIT 24
#define PAGE_SIZE (1<< PAGE_NUM_BIT)
#define PHYSICAL_MEM_SIZE (1<< MEM_NUM_BIT)
#define VIRTUAL_MEM_SIZE (1<< VIRTUAL_NUM_BIT)
#define SWAP_FILE_SIZE (1<<VIRTUAL_NUM_BIT)
#define BIG_MAX_NUM_ACCESSES 5000
#define MED_MAX_NUM_ACCESSES 1000
#define SMOL_MAX_NUM_ACCESSES 10

typedef struct PageTableEntry{
    unsigned int frame : MEM_NUM_BIT-PAGE_NUM_BIT;
    unsigned int valid :1;
    unsigned int unswappable : 1;
    unsigned int read_bit : 1;
    unsigned int write_bit : 1;
    unsigned int reference_bit :1; 
} PageTableEntry; 

typedef struct Frame{
    uint16_t page;
    unsigned int free :1;
} Frame;


typedef struct MMU{
    PageTableEntry * page_table;
    Frame * frame_to_page;
    unsigned int num_frames;
    int fd_swap_file;
    char * buffer;
    uint16_t next;
} MMU;

extern long unsigned int num_disk_accesses;
extern unsigned char verbose;
extern unsigned char enhanced;

void MMU_init(MMU* mmu,char *phy,int swap);
void MMU_exception(MMU* mmu, int pos);
void MMU_writeByte(MMU* mmu, int pos, char c);
char* MMU_readByte(MMU* mmu, int pos);