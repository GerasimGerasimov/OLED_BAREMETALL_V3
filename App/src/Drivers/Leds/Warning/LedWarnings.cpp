#include "LedWarnings.h"

#define LED_WRN_ON   GPIOB->BSRRH = GPIO_Pin_1//10
#define LED_WRN_OFF  GPIOB->BSRRL = GPIO_Pin_1
#define LED_WRN_ST   GPIOB->ODR & GPIO_Pin_1

void LedWarnings::init() {
	setState(0);
}

void LedWarnings::setState(u16 state) {
  switch (state) {
    case 0: 
      LED_WRN_OFF;
      break;
    case 1:
      LED_WRN_ON;
      break;
  };
}
