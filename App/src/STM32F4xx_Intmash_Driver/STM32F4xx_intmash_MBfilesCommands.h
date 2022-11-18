#ifndef FS_MODBUS_H
#define FS_MODBUS_H
#include "Intmash_bastypes.h"
#include "ModbusSlave.h"
#include "STM32F4xx_Intmash_FatFsSD.h"






void FilesOperateProc(void);
void FS_ClearGLSFileName(void);
tU8 ModbusGetDiskSpace(ModbusSlaveType* Slave);
tU8 ModbusGetFilesList(ModbusSlaveType* Slave);
tU8 ModbusReadFile(ModbusSlaveType* Slave);
tU8 ModbusWriteFile(ModbusSlaveType* Slave);
tU8 ModbusRemoveFile(ModbusSlaveType* Slave);


typedef struct FileOperationsStructType
{
 
  //следующее для работы с файлами - не инициализируется
  FILINFO fno; //File status structure сюда будут возвращать данные о статусе файле
  DIR dir; //Directory object structure - о директории, хранящей файл
  FIL f; //File object structure - о самом файле
  
} FileOperationsStructType;
extern FileOperationsStructType* CurrentFileOperationsStruct;


#endif
