#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "common.h"

unsigned int real_data_blocks_num = 0;
map_block_t data_mapping[NUM_DATA_PAGE];
page_t data_region[NUM_DATA_PAGE];

void populate_newdram(ADDRTY addr, SIZETY size, unsigned int index) {


    memcpy(&data_region[index],(const void*)addr,64);
    data_mapping[index].index = index;
    data_mapping[index].old_addr = addr;
    data_mapping[index].new_addr = (ADDRTY)&data_region[index];
    real_data_blocks_num ++ ;

    //unsigned char * tmp_bb = (unsigned char *)((unsigned long)(&code_region[index]));
    //printf("%x, %x, %x\n",tmp_bb[61],tmp_bb[62],tmp_bb[63] );
    printf("this block is %lx\n",addr);
    printf("data region add is %p\n",&data_region[index]);
    //printf("sanity not checking\n");
}
/*
void sanity_chec_inner(){
    //printf("populate %d code blocks/n",real_code_blocks_num);
    //printf("populate %d data blocks/n",real_data_blocks_num);
    for(int i=0;i<real_code_blocks_num;++i){
        //printf("[Code] old address is %lx, => new address is %lx\n",code_mapping[i].old_addr,
        //code_mapping[i].new_addr);
    }
    for(int i=0;i<real_data_blocks_num;++i){
        //printf("[Data]old address is %lx, => new address is %lx\n",data_mapping[i].old_addr,
        //data_mapping[i].new_addr);
    }
    return;


}
*/
