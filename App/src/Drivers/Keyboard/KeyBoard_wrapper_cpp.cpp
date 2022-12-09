#include "KeyBoard_wrapper_cpp.h"
#include "KeyBoard.h"

void KeyBoardUpdateCpp(void) {
  u16 rawcode = KeyBoard::getScanCode();
  KeyBoard::debouncing(rawcode);
  KeyBoard::sendKeyboardMsg();
}



