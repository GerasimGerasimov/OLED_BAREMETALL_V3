#ifndef MODBUSMASTERCONF_H
#define MODBUSMASTERCONF_H
#include "ModbusMaster.h"

#include "Intmash_bastypes.h" 

extern tU8 Flash_read;
extern tU8 CD_read;
extern tU16 scan_code;
extern tU8 write_num_byte;
//extern MBmasterSlotType slot_THCNT_WRITE;
extern MBmasterSlotType slot_RAM_READ;
extern MBmasterSlotType slot_ID_READ;
extern MBmasterSlotType slot_WRITE;
extern MBmasterSlotType slot_CD_READ;
extern MBmasterSlotType slot_FLASH_READ;
extern MBmasterSlotType slot_LOG_READ;
extern MBmasterSlotType slot_RTR;
extern tU16 RTR_size;
extern tU16 RTR_addr;
extern tU16 RetrLinkCnt;

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
extern tU8 fRtrWaitResp;

#endif
