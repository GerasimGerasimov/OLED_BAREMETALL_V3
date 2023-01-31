#ifndef INTERNAL_ADRESATION_H
#define INTERNAL_ADRESATION_H

#include "stm32f4xx.h"

typedef struct {
	s16 Offset;//-1 какая-то ошибка!
	u8 Lenght;
	s16 BitNumber;//-1 нет номера
} TInternalMemAddress;

#endif



