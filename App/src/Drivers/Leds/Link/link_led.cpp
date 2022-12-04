#include "link_led.h"

s16 LinkLED::State = -1;
u16 LinkLED::PULSE = 25;
u16 LinkLED::PAUSE = 100;
/*TODO тут надо сделать индикацию LINK_LED для этого написать новую сущьность
Нет связи:
  светит 0.25 сек
  погашен 1 сек
Есть связь:
  светит 0.25 сек
  погашен 0.25 сек
*/
void LinkLED::init(void){
  LED_LINK_OFF;
}

//-1 есть ошибки
// 0 нет ошибок
void LinkLED::setState(s16 state) {
  State = state;
}

void LinkLED::onTimer(void){
  (State == -1)
    ? (blinkErrState())
    : (blinkNoErrState());
}

void LinkLED::blinkErrState(){  
  PULSE = 10;
  PAUSE = 100;
  blink();
}

void LinkLED::blinkNoErrState(){
  PULSE = 25;
  PAUSE = 25;
  blink();
}

void LinkLED::blink(void) {
  static u32 pause = 0;
  static u32 pulse = PULSE;
  static bool doPulse = true;
  static bool doPause = false;
  if (doPulse)
    (pulse)
      ? (LED_LINK_ON, pulse--)
      : (pause = PAUSE, doPulse = false, doPause = true);
  if (doPause)
    (pause)
      ? (LED_LINK_OFF, pause--)
      : (pulse = PULSE, doPulse = true, doPause = false);       
}


