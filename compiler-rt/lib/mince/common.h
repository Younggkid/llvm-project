#ifndef __COMMON_H__
#define __COMMON_H__


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../../include/sanitizer/mince_interface.h"

typedef unsigned long ADDRTY;
typedef unsigned long SIZETY;

#define NOINLINE
#define INTERFACE_ATTRIBUTE

#define nullptr 0
#define internal_memcpy memcpy

#define LINE_SIZE  64 // the size of cache line
#define PAGE_SIZE  4096 // the size of one page
#define NUM_CODE_PAGE  32
#define NUM_DATA_PAGE  16

typedef struct {
    char memory[LINE_SIZE];
} block_t;

typedef struct {
    __attribute__((aligned(64))) char memory[PAGE_SIZE];
} page_t;

typedef struct {
  ADDRTY new_addr;
  ADDRTY old_addr;
  unsigned int index;
} map_block_t;


extern unsigned int real_data_blocks_num;
extern map_block_t data_mapping[NUM_DATA_PAGE];
extern page_t data_region[NUM_DATA_PAGE];
extern int secret[16];



  // This function is to populate the data of the program
void populate_newdram(unsigned long, unsigned long, unsigned int);

#endif // __RERAND_COMMON_H__