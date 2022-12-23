#include "LedAlarms.h"

#define LED_ALARM_ON   GPIOB->BSRRH = GPIO_Pin_12//11
#define LED_ALARM_OFF  GPIOB->BSRRL = GPIO_Pin_12
#define LED_ALARM_ST   GPIOB->ODR & GPIO_Pin_12

void LedAlarms::init() {
	setState(0);
}

void LedAlarms::setState(u16 state) {
  switch (state) {
    case 0:
	LED_ALARM_OFF;
	break;
    case 1:
	LED_ALARM_ON;
	break;
  };
}
