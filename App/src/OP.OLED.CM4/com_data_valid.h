#ifndef COM_DATA_VALID_H
#define COM_DATA_VALID_H

#include "stm32f4xx.h"

  #ifdef __cplusplus
    extern "C" void setRamDataValidState(void);
    extern "C" bool isRAMDataValid(void);
    extern "C" void resetRamDataValid(void);
  #else
    extern void setRamDataValidState(void);
    extern bool isRAMDataValid(void);
    extern void resetRamDataValid(void);
  #endif
#endif