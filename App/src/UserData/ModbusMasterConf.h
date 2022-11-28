#ifndef MODBUSMASTERCONF_H
#define MODBUSMASTERCONF_H
#include "ModbusMaster.h"

#include "Intmash_bastypes.h" 

extern tU8 write_num_byte;
extern MBmasterSlotType slot_RAM_READ;

  #ifdef __cplusplus
    extern "C" void ModbusMasterInit(void);
    extern "C" void RebootMBMaster(void);
    extern "C" void StopMasterTimer(void);
  #else
    extern void ModbusMasterInit(void);
    extern void RebootMBMaster(void);
    extern void StopMasterTimer(void);
  #endif

extern MBmasterType CommonMaster;
extern MBmasterSlotType PwrRead;

#endif
