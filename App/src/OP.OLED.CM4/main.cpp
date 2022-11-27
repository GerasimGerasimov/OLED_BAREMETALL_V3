/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "init.h"
#include "id.h"
#include "defines.h"
#include "os_resources.h"
#include "resources.h"

#include "ModbusMasterConf.h"
#include "ModbusSlaveConf.h"

#include <string>
#include <app.h>
#include "graphics.h"
#include "display_driver.h"

//#define OVERTURN_X_IN  (GPIOD->IDR & GPIO_Pin_2) 
//#define OVERTURN_Y_IN  (GPIOB->IDR & GPIO_Pin_3) 

int main(void) {  
  BootLoadCmdFillZero();  
  Init();
  LED_WRN_ON;
  
  TDisplayDriver::CleanScreen();
  TGrahics::init();
  TGrahics::outText("Загрузка...", 30, 20, 1, "MSSansSerifBold14");
  TDisplayDriver::out();
  
  ModbusClientInit(); //слейв для проверки
  //ModbusMasterInit(); //мастер  
  __enable_irq(); 
  
  OSResources::init();
  App::init();

  LED_RUN_ON;
  LED_WRN_OFF;
  LED_LINK_OFF;
  LED_ALARM_OFF;
  
  LED2_OFF;
  LED1_ON;
 
  App::run();
}

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
