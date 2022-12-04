#ifndef LINK_LED_H
#define LINK_LED_H

#include "stm32f4xx.h"

#define LED_LINK_ON   GPIOB->BSRRH = GPIO_Pin_10//12
#define LED_LINK_OFF  GPIOB->BSRRL = GPIO_Pin_10
#define LED_LINK_ST   GPIOB->ODR & GPIO_Pin_10

struct LinkLED {
  public:
    static void init();
    static void onTimer();
    static void setState(s16 state);
  private:
    static s16 State;
    static void blinkErrState();
    static void blinkNoErrState();
    static void blink();
    static u16 PULSE;
    static u16 PAUSE;
};

#endif