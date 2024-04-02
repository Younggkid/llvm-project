#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

#ifndef SGX
// #include "interception/interception.h"
//#include "sanitizer_common/sanitizer_common.h"
#endif //SGX

#define __PUSH_CONTEXT_ASM " push %rax\n\t" \
                           " push %rbx\n\t" \
                           " push %rcx\n\t" \
                           " push %rdx\n\t" \
                           " push %rsi\n\t" \
                           " push %rdi\n\t" \
                           " push %r8\n\t"  \
                           " push %r9\n\t"  \
                           " push %r10\n\t" \
                           " push %r11\n\t" \
                           " push %r12\n\t" \
                           " push %r13\n\t" \
                           " pushfq\n\t"    \

#define __POP_CONTEXT_ASM " popfq\n\t"    \
                          " pop %r13\n\t" \
                          " pop %r12\n\t" \
                          " pop %r11\n\t" \
                          " pop %r10\n\t" \
                          " pop %r9\n\t"  \
                          " pop %r8\n\t"  \
                          " pop %rdi\n\t" \
                          " pop %rsi\n\t" \
                          " pop %rdx\n\t" \
                          " pop %rcx\n\t" \
                          " pop %rbx\n\t" \
                          " pop %rax\n\t"

__asm__(
".global __mince_data_addr_translate\n\t"
"__mince_data_addr_translate:\n\t"
__PUSH_CONTEXT_ASM
"call __mince_data_addr_translate_inner\n\t"
"mov %rax, %r14\n\t" //TODO. Exception! it write the result to R14 instead of R15, todo is just for highlighting
__POP_CONTEXT_ASM
"ret" //not jump this time
);

extern "C"
NOINLINE INTERFACE_ATTRIBUTE
ADDRTY __mince_data_addr_translate(void);

extern "C"
NOINLINE INTERFACE_ATTRIBUTE
void __mince_populate(ADDRTY addr1, SIZETY size, unsigned int index) {
  populate_newdram(addr1, size, index);
  return;
}

extern "C"
NOINLINE INTERFACE_ATTRIBUTE
unsigned long __mince_data_addr_translate_inner(void) {
  register ADDRTY req_addr asm ("r15");
  printf("req_addr is %lx\n",req_addr);
  if ((unsigned long)req_addr < (unsigned long)&secret[0] || (unsigned long)req_addr > (unsigned long)&secret[0] + 128 ) return req_addr;
  ADDRTY dst_addr;
  unsigned long req_block = req_addr/64;
  for (int i=0;i<real_data_blocks_num;++i) {
    if (data_mapping[i].old_addr/64 == req_block) 
    {
      dst_addr = data_mapping[i].new_addr+req_addr%64;
      break;
    }
  }
  return dst_addr;
}