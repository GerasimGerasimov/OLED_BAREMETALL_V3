/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "init.h"
#include "id.h"
#include "defines.h"

#include "ModbusMasterConf.h"
#include "ModbusSlaveConf.h"

//#define OVERTURN_X_IN  (GPIOD->IDR & GPIO_Pin_2) 
//#define OVERTURN_Y_IN  (GPIOB->IDR & GPIO_Pin_3) 

int main(void) {  
  BootLoadCmdFillZero();  
  Init();
  //TGrahics::init();
  //App::init();
  //App::run();
  //IDinit();
  
  //ModbusClientInit(); //слейв для проверки
  //ModbusMasterInit(); //мастер  
  
  LED_RUN_ON;
  LED_WRN_OFF;
  LED_LINK_OFF;
  LED_ALARM_OFF;
  
  LED2_OFF;
  LED1_ON;
  
  __enable_irq(); 
  
//------------------------------------------------------------------------------
  static bool tggl = false;
  static u32 i = 200000;
  
  while (1)
  { 
    (i)
      ? i--
      : (tggl = !tggl, i = 200000);
    (tggl)?LED_RUN_ON:LED_RUN_OFF;
  }  
}

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
