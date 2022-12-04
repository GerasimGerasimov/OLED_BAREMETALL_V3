#ifndef MODBUSMASTER_H
#define MODBUSMASTER_H
#include "Intmash_bastypes.h"


#define MB_MASTER_BUF_SIZE  256
#define MB_MASTER_SLOTS_MAX 1

//слот для организации типовой транзакции
typedef struct {  
  tU8 InBuf [MB_MASTER_BUF_SIZE]; 
  void (*OnTimeOut)(void);//процедура которая вызывается если данные не получены
  void (*OnRecieve)(void);//что делаем после получения каких-то данных
  u8 InBufLen;
} MBmasterSlotType;

#endif
