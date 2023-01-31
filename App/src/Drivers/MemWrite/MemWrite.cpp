#include "MemWrite.h"
#include "STM32F4xx_Intmash_Flash.h"
#include "crc16.h"
#include "FlashData.h"
#include "CalibrationData.h"
#include "RAMdata.h"

void MemWrite::writeValueToFlash(TInternalMemAddress& Addr, std::string& value) {
  CopyFlashToTmpBuffer((tU32*)&FLASH_DATA);//скопировал FALSH во временный буфер
  
  u32 v = std::stol(value, nullptr, 16);//значение
  u8* d = (u8*)&FlashTmpBuffer + Addr.Offset;//адрес даных во временном буфере
  
  if (Addr.BitNumber == -1) {
    memcpy(d, (u8*)&v, Addr.Lenght);//скопировал даные во временный буфер
  } else {//сюда пришёл Offset, BitNumber и Lenght
    u8 data = *d;//а тут надо делать битовые операции чтобы не испортить то что вокруг этого бита
    data = (v != 0) ? (data | (1<< Addr.BitNumber)) : ( data & ~(1 << Addr.BitNumber));
    memcpy(d, &data, Addr.Lenght);//но в итоге тоже копирую даные во временный буфер
  }
  //данные находятся во временном буфере, теперь:
  //Подсчитать контрольную сумму временного буфера
  FrameEndCrc16((tU8*)FlashTmpBuffer, FlashTmpBufferSize);
  //буфер готов к записи       
  FlashDataProtectedWrite((tU32*)&FLASH_DATA, (tU32*)&BKFLASH_DATA);
}

void MemWrite::writeValueToCD(TInternalMemAddress& Addr, std::string& value){
  CopyFlashToTmpBuffer((tU32*)&CD_DATA);//скопировал FALSH во временный буфер
  
  u32 v = std::stol(value, nullptr, 16);//значение
  u8* d = (u8*)&FlashTmpBuffer + Addr.Offset;//адрес даных во временном буфере
  
  if (Addr.BitNumber == -1) {
    memcpy(d, (u8*)&v, Addr.Lenght);//скопировал даные во временный буфер
  } else {//сюда пришёл Offset, BitNumber и Lenght
    u8 data = *d;//а тут надо делать битовые операции чтобы не испортить то что вокруг этого бита
    data = (v != 0) ? (data | (1<< Addr.BitNumber)) : ( data & ~(1 << Addr.BitNumber));
    memcpy(d, &data, Addr.Lenght);//но в итоге тоже копирую даные во временный буфер
  }
  //данные находятся во временном буфере, теперь:
  //Подсчитать контрольную сумму временного буфера
  FrameEndCrc16((tU8*)FlashTmpBuffer, FlashTmpBufferSize);
  //буфер готов к записи       
  __disable_irq(); // handles nested interrupt
  FlashSectorWrite((tU32*)&CD_DATA, (tU32*) FlashTmpBuffer);
  __enable_irq(); // handles nested interrupt
}
