#ifndef __FS_PROC_H
#define __FS_PROC_H
#include "diskio.h"
#include "ff.h"
#include <string.h>

#define FAT_NOT_READY 0 //система не проверена и не смонтирована
#define FAT_READY 1     //готова и смонтирована
#define FAT_MOUNT_ERROR 2     //проверена и не может быть смонтирована

extern void fs_proc_run(void);//правильная инициализация диска и файловой системы

extern FATFS fs[1];
extern FATFS *pFS;
extern volatile unsigned char FDiskState;//готовность диска
extern volatile unsigned char FSReady;//1-готовность файловой системы

#endif
