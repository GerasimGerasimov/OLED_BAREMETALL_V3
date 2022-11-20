/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "init.h"
#include "id.h"
#include "defines.h"
#include "os_resources.h"
#include "resources.h"

#include "ModbusMasterConf.h"
#include "ModbusSlaveConf.h"

//#include <iar_dlmalloc.h>
#include <string>


//#define OVERTURN_X_IN  (GPIOD->IDR & GPIO_Pin_2) 
//#define OVERTURN_Y_IN  (GPIOB->IDR & GPIO_Pin_3) 

int main(void) {  
  BootLoadCmdFillZero();  
  Init();
  LED_WRN_ON;
  OSResources::init();
  TInternalResources::init();
  //TGrahics::init();
  //App::init();
  //App::run();
  
  ModbusClientInit(); //слейв дл€ проверки
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
  
  static const char* c = "0123456789ABCDEF";
  std::string str = "unknown  Addr:";//TODO объ€вил Static и апп не падает, без статика не стартует совсем.
  
  while (1)
  { 
    (i)
      ? i--
      : (tggl = !tggl, i = 100000);
    (tggl)
      ? (LED_RUN_ON, str="qwertyt fhnfdffg vhvbhvhgv")
        :(LED_RUN_OFF, str.assign(c, 16));
  }  
}

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
