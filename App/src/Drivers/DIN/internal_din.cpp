#include "internal_din.h"
#include "RAMdata.h"
//входы от платы ASM OP.OLED.ADAPTOR REV3.0.0

#define DIN_0_ST   GPIOC->IDR & GPIO_Pin_4
#define DIN_1_ST   GPIOC->IDR & GPIO_Pin_10
#define DIN_2_ST   GPIOC->IDR & GPIO_Pin_7
#define DIN_3_ST   GPIOC->IDR & GPIO_Pin_5
#define DIN_4_ST   GPIOC->IDR & GPIO_Pin_3
#define DIN_5_ST   GPIOC->IDR & GPIO_Pin_6

void InternalDIN::init(void){

}

void InternalDIN::update(void){
  u16 din = 0;
  din |= ((DIN_0_ST)?1:0) << 0;
  din |= ((DIN_1_ST)?1:0) << 1;
  din |= ((DIN_2_ST)?1:0) << 2;
  din |= ((DIN_3_ST)?1:0) << 3;
  din |= ((DIN_4_ST)?1:0) << 4;
  din |= ((DIN_5_ST)?1:0) << 5;
  RAM_DATA.DIO &= 0xFF00;
  RAM_DATA.DIO |= din;
}