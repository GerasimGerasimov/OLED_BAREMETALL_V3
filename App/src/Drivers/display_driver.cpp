#include "display_driver.h"
#include <iostream>


void TDisplayDriver::setDC(void) {
}

void TDisplayDriver::out(void) {
  /*
    if (dc == NULL) return;
    RECT r;
    HBRUSH br = Br_Empty;
    for (int y = 0; y < VIEW_PORT_MAX_HEIGHT; y++) {
        for (int x = 0; x < VIEW_PORT_MAX_WIDTH; x++) {
            br = (TGrahics::screen[x][y] != 0)
                ? Br_Light
                : Br_Empty;
            r.left = x * DISPLAY_DRRIVER_SCALE;
            r.top = y * DISPLAY_DRRIVER_SCALE;
            r.right = r.left + DISPLAY_DRRIVER_SCALE;
            r.bottom = r.top + DISPLAY_DRRIVER_SCALE;
            FillRect(memDC, &r, br);
        }
    };
    BitBlt(dc, 0, 0, 
        VIEW_PORT_MAX_WIDTH * DISPLAY_DRRIVER_SCALE,
        VIEW_PORT_MAX_HEIGHT * DISPLAY_DRRIVER_SCALE,
        memDC, 0, 0,
    SRCCOPY);
*/
};