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
#include "IniSlotsprops.h"

//#define OVERTURN_X_IN  (GPIOD->IDR & GPIO_Pin_2) 
//#define OVERTURN_Y_IN  (GPIOB->IDR & GPIO_Pin_3) 

struct mallinfo IARHeapInfo;

int main(void) {  
  BootLoadCmdFillZero();  
  Init();
  LED_WRN_ON;
  OSResources::init();
  //TInternalResources::init();
  //TGrahics::init();
  App::init();
  //App::run();
  
  ModbusClientInit(); //����� ��� ��������
  //ModbusMasterInit(); //������  
  
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
  
  //static const char* c = "0123456789ABCDEF";
  //std::string str = "unknown  Addr:";//TODO ������� Static � ��� �� ������, ��� ������� �� �������� ������.
  RAM_DATA.var3 = IniSlotsProps::Devices.size();
  while (1)
  { 
    (i)
      ? i--
      : (tggl = !tggl, i = 100000);
    (tggl)
      ? (LED_RUN_ON/*, str=TInternalResources::getID()*/)
        :(LED_RUN_OFF/*, str.assign(c, 16)*/);
     IARHeapInfo = __iar_dlmallinfo();
     RAM_DATA.var2 = IARHeapInfo.fordblks;
  }  
}

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
