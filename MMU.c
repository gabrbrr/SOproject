#include "MMU.h"

void MMU_init(MMU* mmu){
    mmu->page_table=(PageTableEntry *)mmu->physical_memory;
    mmu->frame_to_page=(uint32_t *) mmu->physical_memory+sizeof(PageTableEntry)*VIRTUAL_MEM_SIZE
    mmu->physical_memory=(char *)mmu->frame_to_page+sizeof(uint32_t)*VIRTUAL_MEM_SIZE/PAGE_SIZE
}


void MMU_writeByte(MMU* mmu, int pos, char c) {
    int pageIndex = pos / PAGE_SIZE;
    int offset = pos % PAGE_SIZE;
    PageTableEntry* page = mmu->page_table[pageIndex];

    if (!page->valid) {
        MMU_exception(mmu, pos);
    }

    mmu->physical_memory[page.frame<<PAGE_NUM_BIT | offset] = c;
    page->write_bit = 1;
}

char* MMU_readByte(MMU* mmu, int pos) {
    int pageIndex = pos / PAGE_SIZE;
    int offset = pos % PAGE_SIZE;
    PageTableEntry* page = mmu->page_table[pageIndex];

    if (!page->valid) {
        MMU_exception(mmu, pos);
    }

    page->read_bit = 1;
    return &mmu->physical_memory[page.frame<<PAGE_NUM_BIT | offset];
}

