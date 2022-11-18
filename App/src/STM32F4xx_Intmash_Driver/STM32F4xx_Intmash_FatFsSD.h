#ifndef __FS_PROC_H
#define __FS_PROC_H
#include "diskio.h"
#include "ff.h"
#include <string.h>

#define FAT_NOT_READY 0 //������� �� ��������� � �� ������������
#define FAT_READY 1     //������ � ������������
#define FAT_MOUNT_ERROR 2     //��������� � �� ����� ���� ������������

extern void fs_proc_run(void);//���������� ������������� ����� � �������� �������

extern FATFS fs[1];
extern FATFS *pFS;
extern volatile unsigned char FDiskState;//���������� �����
extern volatile unsigned char FSReady;//1-���������� �������� �������

#endif
