/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "init.h"
#include "id.h"
#include "defines.h"
#include "os_resources.h"
#include "resources.h"

#include "ModbusMasterConf.h"
#include "ModbusSlaveConf.h"

#include <iar_dlmalloc.h>
#include <string>
#include <app.h>
#include "graphics.h"
#include "display_driver.h"

//#define OVERTURN_X_IN  (GPIOD->IDR & GPIO_Pin_2) 
//#define OVERTURN_Y_IN  (GPIOB->IDR & GPIO_Pin_3) 

struct mallinfo IARHeapInfo;

int main(void) {  
  BootLoadCmdFillZero();  
  Init();
  LED_WRN_ON;
  OSResources::init();
  TDisplayDriver::CleanScreen();
  TGrahics::init();
  TGrahics::outText("Загрузка...", 0, 0, 1, "MSSansSerifBold14");
  TDisplayDriver::out();
  App::init();

  //App::run();
  
  ModbusClientInit(); //слейв для проверки
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
  static u32 i = 1;
  
  //static const char* c = "0123456789ABCDEF";
  //std::string str = "unknown  Addr:";//TODO объявил Static и апп не падает, без статика не стартует совсем.
  //RAM_DATA.var2 = IniSlotsProps::Devices.size();
  while (1)
  { 
    (i)
      ? i--
      : (tggl = !tggl, i = 50000);
    (tggl)
      ? (LED_RUN_ON/*, str=TInternalResources::getID()*/)
        :(LED_RUN_OFF/*, str.assign(c, 16)*/);
     IARHeapInfo = __iar_dlmallinfo();
     RAM_DATA.var3 = IARHeapInfo.uordblks;/* total free space */
     RAM_DATA.var4 = IARHeapInfo.usmblks;/* maximum total allocated space */
     RAM_DATA.var2 ++;
  }  
}

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
