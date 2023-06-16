#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "MMU.h"
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>

char physical_memory[PHYSICAL_MEM_SIZE]; 
long  unsigned int num_disk_accesses=0;
unsigned char verbose=0;
unsigned char enhanced=1;
int main(int  argc, char** argv){
    // Initialize MMU
    MMU mmu;
    char a;
    char * c ;
    char e;
    c=&a;
    int fd = open("miofile.txt", O_CREAT | O_RDWR | O_TRUNC, 0666);
    assert(fd!=-1 && "open swap file");

    MMU_init(&mmu,physical_memory,fd);
     

    
    verbose=1;
    //accesso sequenziale alle prime num_frames*2 pagine dello swap file
    for(int i=0;i<mmu.num_frames*PAGE_SIZE*2;i+=PAGE_SIZE){
        printf("writing byte %c in page %u\n",'A',i/PAGE_SIZE);
        MMU_writeByte(&mmu,i,'A');
        c =MMU_readByte(&mmu,i);
        printf("byte %c read in page %u\n",*c,i/PAGE_SIZE);
    }
    verbose=0;

    printf("Press Enter to continue...\n");
    getchar();


    //modo più semplice per testare se le priorità funzionano è settare direttamente i bit, il puntatore next punta al primo frame nella ram
    //indici pari hanno sempre priorità minore o uguale a quelli dispari
    int l=3*mmu.num_frames*PAGE_SIZE+1; //uso l come numero della pagina nel swap file in modo da accedere ogni volta ad una pagina diversa e non fare hit
    for(unsigned char i=0;i<=0x3;i++){
        for(unsigned char j=i;j<=0X3;j++){
            for(int k=0;k<mmu.num_frames;k++){
                uint16_t ftp=mmu.frame_to_page[k].page;
                if(k%2==0){
                    mmu.page_table[ftp].reference_bit=i>>1 & 0x1;
                    mmu.page_table[ftp].write_bit=i & 0x1;
                } 
                else {
                    mmu.page_table[ftp].reference_bit=j>>1 & 0x1;
                    mmu.page_table[ftp].write_bit=j & 0x1;
                }
            }
            verbose=1;
            printf("\nAccessing 2 new pages not in memory from swap file, Even frames have ref_wr bit %b, Odd frames have ref_wr bit %b\n",i,j);
            c =MMU_readByte(&mmu,l);
            c =MMU_readByte(&mmu,l+PAGE_SIZE);
            l+=2*PAGE_SIZE;
            verbose=0;
        }
    }

    printf("Press Enter to continue...\n");
    getchar();
    
    //Testing memory consistency with swapped pages
    reset(&mmu);
    num_disk_accesses=0;
    srand(4);
    int acc_num=MED_MAX_NUM_ACCESSES;
    char written_string[acc_num+1];
    for(int i=0;i<acc_num;i++){
        e = (char)('A' + (rand() % ('Z' - 'A')));
        if(i<acc_num/2) MMU_writeByte(&mmu,i*PAGE_SIZE%SWAP_FILE_SIZE,e);
        else MMU_writeByte(&mmu,((i-acc_num/2)*PAGE_SIZE +2)%SWAP_FILE_SIZE,e);
        written_string[i]=e;
    }
    
    
    written_string[acc_num]='\0';
    //let's check integrity
    verbose=0;
    char read_string[acc_num+1];
    for(int i=0;i<acc_num;i++){
        if(i<acc_num/2) c =MMU_readByte(&mmu,i*PAGE_SIZE%SWAP_FILE_SIZE);
        else c =MMU_readByte(&mmu,((i-acc_num/2)*PAGE_SIZE+2)%SWAP_FILE_SIZE);
        read_string[i]=*c;
        
    }
    read_string[acc_num]='\0';
    
   
    printf("Memory consistency is %b\n",!strcmp(read_string,written_string));

    printf("Press Enter to continue...\n");
    getchar();

    
    reset(&mmu);
    acc_num=BIG_MAX_NUM_ACCESSES;
    num_disk_accesses=0;
    srand(4);
    for(int i=0;i<acc_num;i++){
        int randomNum = rand() % SWAP_FILE_SIZE;
        *c=((char)i+'A')%('Z'-'A');
        MMU_writeByte(&mmu,randomNum,*c);

        randomNum = rand() % SWAP_FILE_SIZE;
        MMU_readByte(&mmu,randomNum);
    }
    printf("Number of disk accesses for %d writes and %d reads with enhanced: %lu\n",acc_num,acc_num,num_disk_accesses);
    
    
    enhanced=0;  //provo il second classico
    reset(&mmu);
    num_disk_accesses=0;
    srand(4);
    for(int i=0;i<acc_num;i++){
        int randomNum = rand() % SWAP_FILE_SIZE;
        *c=((char)i+'A')%('Z'-'A');
        MMU_writeByte(&mmu,randomNum,*c);

        randomNum = rand() % SWAP_FILE_SIZE;
        MMU_readByte(&mmu,randomNum);
    }
    printf("Number of disk accesses for %d writes and %d reads without enhanced: %lu\n",acc_num,acc_num,num_disk_accesses);

    int ret=close(fd);
    assert(ret!=-1 && "close");

    return 0;
}

void reset(MMU * mmu){
    memset(physical_memory,0,sizeof(physical_memory));
    int ret=close(mmu->fd_swap_file);
    assert(ret!=-1 && "close");

    int fd = open("miofile.txt", O_CREAT | O_RDWR | O_TRUNC, 0666);
    assert(fd!=-1 && "open swap file");

    MMU_init(mmu,physical_memory,fd);
}