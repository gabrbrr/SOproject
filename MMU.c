#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "MMU.h"
#include <string.h>

void MMU_init(MMU* mmu,char * physical_memory,char * swap_file){
    memset(mmu,0,sizeof(MMU));
    mmu->physical_memory=physical_memory;
    mmu->swap_file=swap_file;
    mmu->page_table=(PageTableEntry *)mmu->physical_memory;
    long unsigned int page_table_size=sizeof(PageTableEntry)*VIRTUAL_MEM_SIZE/PAGE_SIZE; 
    mmu->frame_to_page=(uint16_t *) ((char *)mmu->physical_memory+page_table_size);
    int left_memory=PHYSICAL_MEM_SIZE-page_table_size;
    //imposto equazione dove x è il numero di frame sizeof(uint16_t)*x+x*PAGE_SIZE=LEFT_MEMORY      x=left_memory/(PAGE_SIZE + sizeof(uint16))
    mmu->num_frames=left_memory/(PAGE_SIZE + sizeof(uint16_t));
    mmu->buffer=mmu->physical_memory+sizeof(uint16_t)*mmu->num_frames + page_table_size;

}   
void MMU_exception(MMU* mmu, int pos) {
    int pageIndex = pos / PAGE_SIZE; //inizio a controllare dalla pagina 
    unsigned char i=0;
    unsigned char ref_wr;
    // Implemento l'enhanced second chance algorithm  sia i=xy , faccio al massimo 4 cicli cercando in ordine pagine con bit_read,bit_write  00 01 10 11 
    while (1) {
        for(int j=0;j<mmu->num_frames;j++){
            PageTableEntry current = mmu->page_table[mmu->frame_to_page[j]];
            if (current.unswappable) {
                    // Unswappable page error
                    printf("Error: Invalid access to unswappable page!\n");
                    exit(1);
                }
            ref_wr=current.reference_bit <<1 | current.write_bit;
            if(ref_wr<=i){ //vuol dire che ho trovato un frame da togliere
                if(current.write_bit){ //salvo su disco pagina modificata
                    memcpy(&(mmu->swap_file[mmu->frame_to_page[j * PAGE_SIZE]]), &(mmu->buffer[j * PAGE_SIZE]), PAGE_SIZE);
                }
                memcpy(&(mmu->buffer[j * PAGE_SIZE]),&(mmu->swap_file[pageIndex * PAGE_SIZE]),PAGE_SIZE);

                mmu->page_table[pageIndex].frame=j;  //aggiorno indice del frame
                //imposto tutti i flag a 0
                current.valid=0;
                current.write_bit=0;
                current.read_bit=0;
                //imposto il reference bit a 1
                mmu->page_table[pageIndex].reference_bit=1;
                return;
            }
            current.reference_bit=0;

            j = (j + 1) % mmu->num_frames;
        }
        i++;
    }
}

void MMU_writeByte(MMU* mmu, int pos, char c) {
    int pageIndex = pos / PAGE_SIZE;
    printf("\n%d\n",pageIndex);
    fflush(stdout);
    int offset = pos % PAGE_SIZE;
    PageTableEntry* page = mmu->page_table + pageIndex;

    if (!page->valid) {
        MMU_exception(mmu, pos);
    }

    mmu->buffer[page->frame<<PAGE_NUM_BIT | offset] = c;
    page->write_bit = 1;
    page->reference_bit=1;
}

char* MMU_readByte(MMU* mmu, int pos) {
    int pageIndex = pos / PAGE_SIZE;
    int offset = pos % PAGE_SIZE;
    PageTableEntry* page = mmu->page_table + pageIndex;

    if (!page->valid) {
        MMU_exception(mmu, pos);
    }

    page->read_bit = 1;
    page->reference_bit=1;
    return mmu->buffer + (page->frame<<PAGE_NUM_BIT | offset);
}


