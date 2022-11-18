#ifndef I_COM_MASTER_DRIVER_H
#define I_COM_MASTER_DRIVER_H

#include <functional>
#include "stm32f4xx.h"

using TDriverComReadEndHandler = std::function<void(s16 result, u8* reply)>;

#endif