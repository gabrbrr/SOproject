#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "MMU.h"
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>

void MMU_init(MMU* mmu,char * physical_memory,int fd_swap_file){
    memset(mmu,0,sizeof(MMU));
    mmu->fd_swap_file=fd_swap_file;
    mmu->page_table=(PageTableEntry *)physical_memory;
    int page_table_size=sizeof(PageTableEntry)*VIRTUAL_MEM_SIZE/PAGE_SIZE; 
    mmu->frame_to_page=(Frame *) (physical_memory+page_table_size);
    int left_memory=PHYSICAL_MEM_SIZE-page_table_size;
    //x è il numero di frame       x=left_memory/(PAGE_SIZE + sizeof(Frame))
    mmu->num_frames=left_memory/(PAGE_SIZE + sizeof(Frame));
    for(int i=0;i<mmu->num_frames;i++){
        mmu->frame_to_page[i].free=1;
    }
    mmu->buffer=physical_memory+sizeof(Frame)*mmu->num_frames + page_table_size;

}   
void MMU_exception(MMU* mmu, int pos) {
    int pageIndex = pos / PAGE_SIZE; //inizio a controllare dalla pagina 
    off_t off = lseek(mmu->fd_swap_file, pageIndex*PAGE_SIZE, SEEK_SET);
    char i=-1;
    unsigned char ref_wr;
    PageTableEntry *current ;
    // Implemento l'enhanced second chance algorithm, faccio al massimo 5 cicli cercando in ordine pagine con bit_read,bit_write  00 01, in quest'ultimo caso se non trovo nulla inizio a settare reference a 0 e ricomincio 
    while (1) {
        uint16_t next=mmu->next;
        for(uint16_t j=next; (j+1)%mmu->num_frames!=next;j=(j+1)%mmu->num_frames){ //condizione per farmi un ciclo su tutti i frame a partire da next
            if(mmu->frame_to_page[j].free){
                //memcpy(&(mmu->buffer[j * PAGE_SIZE]),&(mmu->swap_file[pageIndex * PAGE_SIZE]),PAGE_SIZE);
                read(mmu->fd_swap_file,&(mmu->buffer[j * PAGE_SIZE]),PAGE_SIZE);
                if(verbose) printf("Swapping mapped page %d to %d frame from memory\n",pageIndex,j);
                
                mmu->page_table[pageIndex].frame=j;  //aggiorno indice nella page table
                mmu->page_table[pageIndex].reference_bit=1; //imposto il reference bit a 1
                mmu->page_table[pageIndex].valid=1; // la pagina caricata è valida
                
                
                mmu->frame_to_page[j].page=pageIndex;//aggiorno riferimento alla pagina
                mmu->frame_to_page[j].free=0;
               
                mmu->next=(j+1) %mmu->num_frames;
                num_disk_accesses++;
                
                return;
            }
            current= &mmu->page_table[mmu->frame_to_page[j].page];
            if (current->unswappable) {
                    // Unswappable page error
                    printf("Error: Invalid access to unswappable page!\n");
                    exit(1);
            }
            
            if (enhanced) ref_wr=current->reference_bit <<1 | current->write_bit;
            else ref_wr=current->reference_bit;
            
            if(ref_wr<=i){ //vuol dire che ho trovato un frame da togliere
                if(current->write_bit){ //salvo su disco pagina modificata
                    off= lseek(mmu->fd_swap_file, mmu->frame_to_page[j].page*PAGE_SIZE, SEEK_SET);
                    write(mmu->fd_swap_file,&(mmu->buffer[j * PAGE_SIZE]),PAGE_SIZE);
                    if (verbose) printf("Swapping back frame %d in memory to mapped page %d in swap file\n",j,mmu->frame_to_page[j].page);
                    num_disk_accesses++;
                }
                
                if (verbose) printf("Swapping mapped page %d to frame %d in memory\n",pageIndex,j);
                
                off = lseek(mmu->fd_swap_file, pageIndex*PAGE_SIZE, SEEK_SET);
                read(mmu->fd_swap_file,&(mmu->buffer[j * PAGE_SIZE]),PAGE_SIZE);

                mmu->page_table[pageIndex].frame=j;  //aggiorno indice del frame
                mmu->page_table[pageIndex].valid=1; // la pagina caricata è valida
                mmu->page_table[pageIndex].reference_bit=1;
                mmu->frame_to_page[j].page=pageIndex; //aggiorno indice della pagina
                //imposto tutti i flag a 0
                current->valid=0;
                current->write_bit=0;
                current->read_bit=0;
                current->reference_bit=0;
               
                //imposto il reference bit a 1
                mmu->page_table[pageIndex].reference_bit=1;
                //aggiorno il puntatore al prossimo frame da controllare
                mmu->next=(j+1) %mmu->num_frames;
                num_disk_accesses++;
                return;
            }
            if((enhanced && i==0x1) || (!enhanced && i==0x0)) current->reference_bit=0; //azzero reference bit se mi trovo a priorità 01 e non ho trovato nulla 
        }
        if(enhanced) i=(i+1)%0x2;
        else i=0;
    }
}

void MMU_writeByte(MMU* mmu, int pos, char c) {
    assert(pos<VIRTUAL_MEM_SIZE && "SIGSEV c:");
    int pageIndex = pos / PAGE_SIZE;
    int offset = pos % PAGE_SIZE;
    PageTableEntry* page = mmu->page_table+pageIndex;

    if (!page->valid) {
        if(verbose) printf("Write Page Fault!\n");
        MMU_exception(mmu, pos);
    }
    mmu->buffer[page->frame<<PAGE_NUM_BIT | offset] = c;
    
    page->write_bit = 1;
}

char* MMU_readByte(MMU* mmu, int pos) {
    int pageIndex = pos / PAGE_SIZE;
    int offset = pos % PAGE_SIZE;
    PageTableEntry* page = mmu->page_table + pageIndex;

    if (!page->valid) {
        if(verbose) printf("Read Page Fault!\n");
        MMU_exception(mmu, pos);
    }

    page->read_bit = 1;
    return &mmu->buffer[page->frame<<PAGE_NUM_BIT | offset];
}


