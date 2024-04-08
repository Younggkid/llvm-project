#ifndef SANITIZER_MINCE_INTERFACE_H
#define SANITIZER_MINCE_INTERFACE_H

#include <sanitizer/common_interface_defs.h>

typedef unsigned long ADDRTY;
typedef unsigned long SIZETY;

#ifdef __cplusplus
extern "C" {
#endif

    void __mince_populate(unsigned long, unsigned long, unsigned int);

#ifdef __cplusplus
}
#endif

#endif