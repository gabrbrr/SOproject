#include "MMU.h"
#include <stdio.h>
#include <string.h>

char swap_file[SWAP_FILE_SIZE];
char physical_memory[PHYSICAL_MEM_SIZE]; 

int main(int  argc, char** argv){
    // Initialize MMU
    MMU mmu;
    MMU_init(&mmu,physical_memory,swap_file);
    //accesso sequenziale ad ogni pagina del file
    // for(int i=0;i<VIRTUAL_MEM_SIZE;i+=PAGE_SIZE){
    //     printf("byte %c written in page %u\n",'A',i/PAGE_SIZE);
    //     MMU_writeByte(&mmu,i,'A');
    //     char * c =MMU_readByte(&mmu,i);
    //     printf("byte %c read in page %u\n",*c,i/PAGE_SIZE);
    // }
    // MMU_init(&mmu,physical_memory,swap_file);
    
    //Mostro che preferisce evictare 00 piuttosto che 01
    //Mostro che preferisce evictare 01 piuttosto che 10
    //Mostro che preferisce evictare 10 piuttosto che 11
    //Mostro che preferisce evictare 00 piuttosto che 01
    for(int i=0;i<4*PAGE_SIZE;i+=PAGE_SIZE){
        printf("byte %c written in page %u\n",'A',i/PAGE_SIZE);
        if(i<mmu.num_frames/2) {
            MMU_writeByte(&mmu,i,'A');
            printf("byte %c written in page %u\n",'A',i/PAGE_SIZE);
        }
        else{
            char * c =MMU_readByte(&mmu,i);
            printf("byte %c read in page %u\n",*c,i/PAGE_SIZE);
        }
    }
    MMU_writeByte(&mmu,mmu.num_frames*PAGE_SIZE+5,'B');
    char * c =MMU_readByte(&mmu,mmu.num_frames*PAGE_SIZE+5);
    printf("byte %c read in page %u\n",*c,1/PAGE_SIZE);
   

    return 0;
}