#include "CmdSenderTimerWrapper_cpp.h"
#include "stm32f4xx.h"
#include "CmdSender.h"

void CmdSenderBusyControlCpp(void) {
  static u16 delay = 0;
  (delay)
    ? delay--
    :(CmdSender::onTimer(),
      delay=10);
}