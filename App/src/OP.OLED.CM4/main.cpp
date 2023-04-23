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
#include "LedAlarms.h"
#include "LedWarnings.h"
#include "LEDs.h"
#include "internal_din.h"
#include "internal_dout.h"

/*TODO поскольку ID сейчс задаётся через ресурсы, не забывать менять счётчик версий*/
volatile const char FirmWareInfo[]="OLED.EFI2 v3.1.1 23.04.2023 www.intmash.ru";

#include "STM32F4xx_Intmash_Flash.h"

int main(void) {  
  BootLoadCmdFillZero();  
  Init();

  TDisplayDriver::CleanScreen();
  TGraphics::init();
  TGraphics::outText("Загрузка...", 0, 0, 1, "Verdana12");
  TDisplayDriver::out();
  
  ModbusMasterInit(); //мастер  
  ModbusClientInit(); //слейв для проверки
  __enable_irq();
  
  if(FlashStatus.Bits.FLASH_DATA_ERR){
    TGraphics::outText("FLASH_DATA_ERR", 0, 24, 1, "Verdana12");
    TDisplayDriver::out();  
  }
  
  LinkLED::init();
  LedWarnings::init();
  LedAlarms::init();
  InternalDIN::init();
  InternalDOUT::init();
  
  OSResources::init();
  (App::init()) 
    ?(App::run())
    :(App::error());
}
