#ifndef RESOURCES_WRAPPER_H
#define RESOURCES_WRAPPER_H

#include "stm32f4xx.h"// basic types

typedef struct ItemProps {
    char* Addr;
    int Size;
} TItemProps;

  #ifdef __cplusplus
    extern "C" char * getIDfromResources(void);
    extern "C" TItemProps getIDPropsFromRes(void);
  #else
    extern char * getIDfromResources(void);
    extern TItemProps getIDPropsFromRes(void);
  #endif
#endif
