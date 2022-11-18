#ifndef _BKUP_SRAM_H
#define _BKUP_SRAM_H

#include "stm32f4xx.h"

void BkpSram_Configuration(void);

u8 BkpSramWrite( u8 *Src, u16 BytesNum, u16 BkpSramAddr);
u8 BkpSramRead( u8 *Dst, u16 BytesNum, u16 BkpSramAddr);

#endif
