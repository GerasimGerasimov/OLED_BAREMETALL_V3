#include "display_driver.h"
#include "ssd1305.h"
#include "display_dc.h"
#include <iostream>

//максимальные позиции по х и по у для дисплея
static const u8 MAX_END_X = 127;
static const u8 MAX_END_Y = 63;

void TDisplayDriver::setDC(void) {
}

void TDisplayDriver::out(void) {
  ssd1305_rst_first();
  DMA_Cmd(DMA2_Stream3, DISABLE);  //TX
  DMA2->LIFCR = (uint32_t) (DMA_FLAG_FEIF3 | DMA_FLAG_DMEIF3 | DMA_FLAG_TEIF3 | DMA_FLAG_HTIF3 | DMA_FLAG_TCIF3);
  DMA2_Stream3->NDTR = 1024;
  DMA_Cmd(DMA2_Stream3, ENABLE); //TX
    
  if ( DMA2_Stream3->NDTR!=0)
   {
     while (DMA2_Stream3->NDTR);
   }
  ssd1305_rst_second();
};

/*функция очистки экрана*/
void TDisplayDriver::CleanScreen(void) {
  for (int i=0; i<8; i++) 
  {
      for (int j=0; j<(MAX_END_X+1); j++)
      { framebuffer[i][j]=0;}
  }
}