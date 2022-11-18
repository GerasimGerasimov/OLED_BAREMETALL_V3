#ifndef MODBUSCONF_H
#define MODBUSCONF_H

#include "ModbusSlave.h"
#include "STM32F4xx_Intmash_USART.h"

  #ifdef __cplusplus
    extern "C" void ModbusClientInit(void);
    extern "C" void ModbusSlaveProc(void);
    extern "C" void RCMD_Rst_Timer_run(void);
    extern "C" void BootLoadCmdFillZero(void);
  #else
    extern void ModbusClientInit(void);
    extern void ModbusSlaveProc(void);
    extern void RCMD_Rst_Timer_run(void);
    extern void BootLoadCmdFillZero(void);
  #endif

extern ModbusSlaveType RS485slave;
extern Intmash_Usart UART1toRS485;

#endif
