#include "internal_dout.h"
#include "RAMdata.h"

#define DOUT_0_ON   GPIOC->BSRRH = GPIO_Pin_9
#define DOUT_0_OFF  GPIOC->BSRRL = GPIO_Pin_9
#define DOUT_0_ST   GPIOC->ODR & GPIO_Pin_9

#define DOUT_1_ON   GPIOC->BSRRH = GPIO_Pin_8
#define DOUT_1_OFF  GPIOC->BSRRL = GPIO_Pin_8
#define DOUT_1_ST   GPIOC->ODR & GPIO_Pin_8

#define DOUT0BIT (1<<8)
#define DOUT1BIT (1<<9)

void InternalDOUT::init(void){
  RAM_DATA.DIO &= 0x00FF;;//старший байт для вЫходов
}

void InternalDOUT::update(void){
  (RAM_DATA.DIO & DOUT0BIT) ? DOUT_0_OFF : DOUT_0_ON;
  (RAM_DATA.DIO & DOUT1BIT) ? DOUT_1_OFF : DOUT_1_ON;
}

void InternalDOUT::setState(u16 state) {
  switch (state) {
    case 0:
	RAM_DATA.DIO &= ~DOUT0BIT;
	break;
    case 1:
	RAM_DATA.DIO |= DOUT0BIT;
	break;
  };
}