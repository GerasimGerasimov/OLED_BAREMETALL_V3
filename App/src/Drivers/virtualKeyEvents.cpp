#include "virtualKeyEvents.h"
#include "msg.h"
#include "RAMdata.h"

void scanVirtualKeyCode() {
  if (RAM_DATA.KeyCode) {
    u32 code = RAM_DATA.KeyCode;
    Msg::send_message((u32)EventSrc::KEYBOARD, code, 0);
    RAM_DATA.KeyCode = 0;
  }

}