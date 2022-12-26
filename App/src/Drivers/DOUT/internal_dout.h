#ifndef INTERNAL_DOUT_H
#define INTERNAL_DOUT_H

#include "stm32f4xx.h"

struct InternalDOUT {
  public:
    static void init();
    static void update();
};

#endif