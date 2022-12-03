#ifndef MODBUSMASTERCONF_H
#define MODBUSMASTERCONF_H
#include "ModbusMaster.h"
#include "Intmash_bastypes.h" 

extern MBmasterSlotType SlotMaster;

  #ifdef __cplusplus
    extern "C" void ModbusMasterInit(void);
    extern "C" void ModbusMasterSend(u8* data, u8 len);
    extern "C" void RebootMBMaster(void);
    extern "C" void StopMasterTimer(void);
  #else
    extern void ModbusMasterInit(void);
    extern void ModbusMasterSend(u8* data, u8 len);
    extern void RebootMBMaster(void);
    extern void StopMasterTimer(void);
  #endif

#endif
