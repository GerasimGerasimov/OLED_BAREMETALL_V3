/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "init.h"
#include "id.h"
#include "os_resources.h"
#include "resources.h"

#include "ModbusMasterConf.h"
#include "ModbusSlaveConf.h"

#include <string>
#include <app.h>
#include "graphics.h"
#include "display_driver.h"
#include "link_led.h"
#include "LEDs.h"

int main(void) {  
  BootLoadCmdFillZero();  
  Init();
  LED_WRN_ON;
  
  ModbusClientInit(); //слейв для проверки
  __enable_irq(); 
  TDisplayDriver::CleanScreen();
  TGrahics::init();
  TGrahics::outText("Загрузка...", 30, 20, 1, "MSSansSerifBold14");
  TDisplayDriver::out();
  
  ModbusMasterInit(); //мастер  
  
  OSResources::init();
  App::init();

  LED_RUN_ON;
  LED_WRN_OFF;
  LinkLED::init();
  LED_ALARM_OFF;
  
  LED2_OFF;
  LED1_ON;
 
  App::run();
}

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
