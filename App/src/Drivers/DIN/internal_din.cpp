#include "internal_din.h"

#define DIN_0_ST   GPIOC->IDR & GPIO_Pin_4
#define DIN_1_ST   GPIOC->IDR & GPIO_Pin_10
#define DIN_2_ST   GPIOC->IDR & GPIO_Pin_7
#define DIN_3_ST   GPIOC->IDR & GPIO_Pin_5
#define DIN_4_ST   GPIOC->IDR & GPIO_Pin_3
#define DIN_5_ST   GPIOC->IDR & GPIO_Pin_6

void InternalDIN::init(void){

}

void InternalDIN::update(void){

}