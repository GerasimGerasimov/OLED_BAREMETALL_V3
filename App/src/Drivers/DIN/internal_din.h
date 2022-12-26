#ifndef INTERNAL_DIN_H
#define INTERNAL_DIN_H

#include "stm32f4xx.h"

struct InternalDIN {
  public:
    static void init();
    static void update();
};

#endif