#include "MMU.h"
#include <stdio.h>
#include <string.h>

char swap_file[SWAP_FILE_SIZE];
char physical_memory[PHYSICAL_MEM_SIZE]; 
long  unsigned int disk_access=0;
unsigned char verbose=0;
int main(int  argc, char** argv){
    // Initialize MMU
    
    MMU mmu;
    char a;
    char * c ;
    c=&a;
    MMU_init(&mmu,physical_memory,swap_file);
     

    
    verbose=1;
    //accesso sequenziale alle prime num_frames*2 pagine dello swap file
    for(int i=0;i<mmu.num_frames*PAGE_SIZE*2;i+=PAGE_SIZE){
        printf("byte %c written in page %u\n",'A',i/PAGE_SIZE);
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
                //mmu.page_table[ftp].reference_bit=0;
                if(k%2==0){
                    mmu.page_table[ftp].reference_bit=i>>1 & 0x1;
                    mmu.page_table[ftp].write_bit=i & 0x1;
                } 
                else {
                    mmu.page_table[ftp].reference_bit=j>>1 & 0x1;
                    mmu.page_table[ftp].write_bit=j & 0x1;
                }
            }
            verbose =1;
            printf("\nAccessing 2 new pages not in memory from swap file, Even frames have ref_wr bit %b, Odd frames have ref_wr bit %b\n",i,j);
            c =MMU_readByte(&mmu,l);
            c =MMU_readByte(&mmu,l+=PAGE_SIZE);
            verbose=0;
            l+=2*PAGE_SIZE;
        }
    }

    printf("Press Enter to continue...\n");
    getchar();
    
    verbose=0;
    reset(&mmu);
    disk_access=0;
    srand(4);
    int acc_num=BIG_MAX_NUM_ACCESSES;
    char written_string[acc_num+1];
    for(int i=0;i<acc_num;i++){
        int randomNum = rand() % SWAP_FILE_SIZE;
        *c=((char)i+'A')%('Z'-'A');
        MMU_writeByte(&mmu,randomNum,*c);
        written_string[i]=*c;

    }
    written_string[acc_num]='\0';
    printf("Number of disk accesses for %d writes: %lu\n",acc_num,disk_access);
    //let's check integrity
    char read_string[acc_num+1];
    srand(4);
    for(int i=0;i<acc_num;i++){
        int randomNum = rand() % SWAP_FILE_SIZE;
        c =MMU_readByte(&mmu,randomNum);
        read_string[i]=*c;
        
    }
    read_string[acc_num]='\0';
    printf("Memory consistency is %b\n",!strcmp(read_string,written_string));
    

    

    

    
    
   

    return 0;
}

void reset(MMU * mmu){
    memset(physical_memory,0,sizeof(physical_memory));
    memset(swap_file,0,sizeof(swap_file));
    MMU_init(mmu,physical_memory,swap_file);
}