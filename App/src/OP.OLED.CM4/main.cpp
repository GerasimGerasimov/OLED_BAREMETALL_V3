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

int main(void) {  
  BootLoadCmdFillZero();  
  Init();

  TDisplayDriver::CleanScreen();
  TGrahics::init();
  TGrahics::outText("Загрузка...", 0, 0, 1, "Verdana12");
  TDisplayDriver::out();
  
  ModbusMasterInit(); //мастер  
  ModbusClientInit(); //слейв для проверки
  __enable_irq();
  
  LinkLED::init();
  LedWarnings::init();
  LedAlarms::init();
  InternalDIN::init();
  InternalDOUT::init();
  
  OSResources::init();
  App::init(); 
  App::run();
}
