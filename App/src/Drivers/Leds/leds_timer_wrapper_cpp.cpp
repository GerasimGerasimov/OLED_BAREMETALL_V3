#include "leds_timer_wrapper_cpp.h"
#include "stm32f4xx.h"
#include "link_led.h"

void LedsTimerTickCpp(void) {
  static u16 delay = 0;
  (delay)
    ? delay--
    :(LinkLED::onTimer(),
      delay=10);
}