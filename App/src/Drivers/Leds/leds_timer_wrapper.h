#ifndef LEDS_TIMER_WRAPPER_H
#define LEDS_TIMER_WRAPPER_H

  #ifdef __cplusplus
    extern "C" void LedsTimerTick(void);
  #else
    extern void LedsTimerTick(void);
  #endif

#endif