#include "display_dc.h"

u8 framebuffer[8][128]; //для передачи по SPI 

u32 getFrameBufferAddr(void) {
  return (u32) & framebuffer;
}