#ifndef LEDS_H
#define LEDS_H

#include "stm32f4xx.h"

//для работы со светиками
#define LED_RUN_ON   GPIOB->BSRRH = GPIO_Pin_11//13
#define LED_RUN_OFF  GPIOB->BSRRL = GPIO_Pin_11
#define LED_RUN_ST   GPIOB->ODR & GPIO_Pin_11

#define LED_WRN_ON   GPIOB->BSRRH = GPIO_Pin_1//10
#define LED_WRN_OFF  GPIOB->BSRRL = GPIO_Pin_1
#define LED_WRN_ST   GPIOB->ODR & GPIO_Pin_1

#define LED_ALARM_ON   GPIOB->BSRRH = GPIO_Pin_12//11
#define LED_ALARM_OFF  GPIOB->BSRRL = GPIO_Pin_12
#define LED_ALARM_ST   GPIOB->ODR & GPIO_Pin_12

#define LED1_ON   GPIOA->BSRRL = GPIO_Pin_11
#define LED1_OFF  GPIOA->BSRRH = GPIO_Pin_11
#define LED1_ST   (GPIOA->ODR & GPIO_Pin_11)
#define LED2_ON   GPIOA->BSRRL = GPIO_Pin_0
#define LED2_OFF  GPIOA->BSRRH = GPIO_Pin_0
#define LED2_ST   (GPIOA->ODR & GPIO_Pin_0)

  #ifdef __cplusplus
    extern "C" void check_leds(void);
    extern "C" bool isWarnings(void);
    extern "C" bool isAlarms(void);
    extern "C" void initAlarmsPage(void);
    extern "C" void sortPageByAlarms(void);
  #else
    extern void check_leds(void);
    extern bool isWarnings(void);
    extern bool isAlarms(void);
    extern void initAlarmsPage(void);    
    extern void sortPageByAlarms(void);
  #endif
#endif