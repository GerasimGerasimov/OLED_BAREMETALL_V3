#include "timer_wrapper_cpp.h"
#include "stm32f4xx.h"
#include "msg.h"

void sendTimerMessageCpp(void) {
  static u16 delay = 0;
  (delay)
    ? delay--
    :(Msg::send_message((u32)EventSrc::TIMER, 0, 0),
      delay=100);
  
}