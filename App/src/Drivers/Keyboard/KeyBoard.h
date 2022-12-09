#ifndef KEY_BOARD_H
#define KEY_BOARD_H

#include "stm32f4xx.h"

struct KeyBoard {
  public:
    static u16 getScanCode(void);
    static void debouncing (u16 rawcode);
    static void sendKeyboardMsg(void);
    static u16 ScanCode;
  private:
    static void prepareToRepeat(void);
    static void repeating(void);
    static u16 repeatState;
    static u16 repeatDelay;
};

#endif