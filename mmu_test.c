#include "MMU.h"
#include <stdio.h>

char swap_file[SWAP_FILE_SIZE];
char physical_memory[PHYSICAL_MEM_SIZE]; 

int main(int  argc, char** argv){
    // Initialize MMU
    MMU mmu;
    MMU_init(&mmu,physical_memory,swap_file);
    printf("sizeof pagetableentry: %ld,num_pages %d,physical_memory %p,buffer %p, page tb %p, frame to page %p",sizeof(PageTableEntry),mmu.num_frames,physical_memory,mmu.buffer,mmu.page_table,mmu.frame_to_page);
    for(int i=0;i<SWAP_FILE_SIZE;i+=PAGE_SIZE){
        MMU_writeByte(&mmu,i,'A');
        // char * c =MMU_readByte(&mmu,i);
        // printf("byte %c read in position %u",*c,i);
    }
    return 0;
}