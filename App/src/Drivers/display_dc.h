#ifndef _DISPLAY_DC_H
#define _DISPLAY_DC_H

#include "stm32f4xx.h"

extern u8 framebuffer[8][128];

  #ifdef __cplusplus
    extern "C" u32 getFrameBufferAddr(void);
  #else
    extern u32 getFrameBufferAddr(void);
  #endif

#endif