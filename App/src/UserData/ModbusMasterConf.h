#ifndef MODBUSMASTERCONF_H
#define MODBUSMASTERCONF_H
#include "ModbusMaster.h"
#include "Intmash_bastypes.h" 

extern MBmasterSlotType SlotMaster;

  #ifdef __cplusplus
    extern "C" void ModbusMasterInit(void);
    extern "C" void ModbusMasterSetCondition(u16 timeOut, u8* replyPtr);
    extern "C" void ModbusMasterSend(u8* data, u8 len);
    extern "C" void RebootMBMaster(void);
  #else
    extern void ModbusMasterInit(void);
    extern void ModbusMasterSetCondition(u16 timeOut, u8* replyPtr);
    extern void ModbusMasterSend(u8* data, u8 len);
    extern void RebootMBMaster(void);
  #endif

#endif
