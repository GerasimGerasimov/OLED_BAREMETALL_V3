#ifndef SLOT_HANDLER_TYPE_H
#define SLOT_HANDLER_TYPE_H

#include "stm32f4xx.h"

typedef struct {
	u8* InputBuf;//буфер сообщения для приёма сообщений
	u8 InputBufValidBytes;
	u16 StartAddrOffset;
	u16 LastAddrOffset;
} TSlotHandlerArsg;

#endif