#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H_H

#include <string>
#include <vector>
#include "stm32f4xx.h"
#include "graphics.h"

struct TDisplayDriver {
  public:
    static void setDC(void);
    static void out(void);
    static void CleanScreen(void);
};

#endif
