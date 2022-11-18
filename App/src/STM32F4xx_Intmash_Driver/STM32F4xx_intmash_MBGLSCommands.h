#ifndef STM32F4xx_intmash_MBGLSCommands_H
#define STM32F4xx_intmash_MBGLSCommands_H
#include "STM32F4xx_intmash_MBfilesCommands.h"



void FS_ClearGLSFileName(void);

tU8 ModbusSetGLSFile(ModbusSlaveType* Slave);
tU8 ModbusGetGLSFile(ModbusSlaveType* Slave);
tU8 ModbusSetGLSFileName(ModbusSlaveType* Slave);





#endif
