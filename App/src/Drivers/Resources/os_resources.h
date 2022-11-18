#ifndef OS_RESOURCES_H
#define OS_RESOURCES_H

#include "resources.h"
#include "stm32f4xx.h"
#include <vector>

struct OSResources {
  public:
    static void init();
    static const u8* getRes(void);
private:
    static u8* res;
};

#endif