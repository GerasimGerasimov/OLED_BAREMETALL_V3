#ifndef MODBUSMASTER_H
#define MODBUSMASTER_H
#include "Intmash_bastypes.h"


#define MB_MASTER_BUF_SIZE  256
#define MB_MASTER_SLOTS_MAX 1

//���� ��� ����������� ������� ����������
typedef struct {  
  tU8 InBuf [MB_MASTER_BUF_SIZE]; 
  void (*OnTimeOut)(void);//��������� ������� ���������� ���� ������ �� ��������
  void (*OnRecieve)(void);//��� ������ ����� ��������� �����-�� ������
  u8 InBufLen;
} MBmasterSlotType;

#endif
