#pragma once
#include <stdint.h>
#define PAGE_NUM_BIT 12
#define MEM_NUM_BIT 20
#define VIRTUAL_NUM_BIT 24
#define PAGE_SIZE (1<< PAGE_NUM_BIT)
#define PHYSICAL_MEM_SIZE (1<< MEM_NUM_BIT)
#define VIRTUAL_MEM_SIZE (1<< VIRTUAL_NUM_BIT)
#define SWAP_FILE_SIZE (1<<VIRTUAL_NUM_BIT)

typedef struct PageTableEntry{
    unsigned int frame : MEM_NUM_BIT-PAGE_NUM_BIT;
    unsigned int valid :1;
    unsigned int unswappable : 1;
    unsigned int read_bit : 1;
    unsigned int write_bit : 1;
    unsigned int reference_bit :1; // solo write bit e read bit non bastano nè per enhances secon chance nè classico, classico: reference bit potresti mettere ref=read_bit | write_bit (ultimo accesso o in lettura o scrittura) ma quando scandisci e trovi 1, imposti entrambi a 0? allora quando andrò a fare swapping non mi ricordero di fare write back, se azzero solo bit_read, se avrò bit_write la pagina non verrà mai swappata
} PageTableEntry;

//1048576   4096*4 = 16384   

typedef struct Frame{
    uint16_t page;
    unsigned int free;
} Frame;


typedef struct MMU{
    PageTableEntry * page_table;
    Frame * frame_to_page;
    unsigned int num_frames;
    char * swap_file;
    char * buffer;
    uint16_t next;
} MMU;



void MMU_init(MMU* mmu,char *phy,char * swap);
void MMU_exception(MMU* mmu, int pos);
void MMU_writeByte(MMU* mmu, int pos, char c);
char* MMU_readByte(MMU* mmu, int pos);