#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PAGE_NUM_BIT 12
#define MEM_NUM_BIT 20
#define VIRTUAL_NUM_BIT 24
#define PAGE_SIZE 1<<PAGE_NUM_BIT
#define PHYSICAL_MEM_SIZE 1<< MEM_NUM_BIT
#define VIRTUAL_MEM_SIZE 1<< VIRTUAL_NUM_BIT
#define SWAP_FILE_SIZE VIRTUAL_MEM_SIZE

typedef struct {
    unsigned int valid :1;
    unsigned int unswappable : 1;
    unsigned int read_bit : 1;
    unsigned int write_bit : 1;
    unsigned int frame : MEM_NUM_BIT;
} PageTableEntry;

typedef struct {
    PageTableEntry page_table[VIRTUAL_MEM_SIZE / PAGE_SIZE];
    char physical_memory[PHYSICAL_MEM_SIZE];
    PageTableEntry * page_table;
    char swap_file[SWAP_FILE_SIZE];
    uint32_t * frame_to_page;
} MMU;


void MMU_init(MMU* mmu);
void MMU_exception(MMU* mmu, int pos);
void MMU_writeByte(MMU* mmu, int pos, char c);
char* MMU_readByte(MMU* mmu, int pos);
