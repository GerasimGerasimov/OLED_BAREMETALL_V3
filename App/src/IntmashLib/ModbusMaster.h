#ifndef MODBUSMASTER_H
#define MODBUSMASTER_H
#include "Intmash_bastypes.h"

//слот для организации типовой транзакции
typedef struct {  
  void (*OnTimeOut)(void);//процедура которая вызывается если данные не получены
  void (*OnRecieve)(void);//что делаем после получения каких-то данных
  u8 InBufLen;//сколько данных принято
} MBmasterSlotType;

#endif
