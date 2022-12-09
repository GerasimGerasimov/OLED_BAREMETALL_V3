#ifndef KEY_BOARD_H
#define KEY_BOARD_H

#include "stm32f4xx.h"

struct KeyBoard {
  public:
    static u16 getScanCode(void);
    static void debouncing (u16 rawcode);
    static void sendKeyboardMsg(void);
    static u16 ScanCode;
};

#endif