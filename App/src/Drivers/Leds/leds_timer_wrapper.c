#include "leds_timer_wrapper.h"
#include "leds_timer_wrapper_cpp.h"

void LedsTimerTick(void) {
  LedsTimerTickCpp();
}