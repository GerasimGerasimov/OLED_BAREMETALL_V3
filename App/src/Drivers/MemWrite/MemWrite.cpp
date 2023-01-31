#include "MemWrite.h"
#include "STM32F4xx_Intmash_Flash.h"
#include "crc16.h"
#include "FlashData.h"
#include "CalibrationData.h"
#include "RAMdata.h"

void MemWrite::writeValueToFlash(TInternalMemAddress& Addr, std::string& value) {
  CopyFlashToTmpBuffer((tU32*)&FLASH_DATA);//���������� FALSH �� ��������� �����
  
  u32 v = std::stol(value, nullptr, 16);//��������
  u8* d = (u8*)&FlashTmpBuffer + Addr.Offset;//����� ����� �� ��������� ������
  
  if (Addr.BitNumber == -1) {
    memcpy(d, (u8*)&v, Addr.Lenght);//���������� ����� �� ��������� �����
  } else {//���� ������ Offset, BitNumber � Lenght
    u8 data = *d;//� ��� ���� ������ ������� �������� ����� �� ��������� �� ��� ������ ����� ����
    data = (v != 0) ? (data | (1<< Addr.BitNumber)) : ( data & ~(1 << Addr.BitNumber));
    memcpy(d, &data, Addr.Lenght);//�� � ����� ���� ������� ����� �� ��������� �����
  }
  //������ ��������� �� ��������� ������, ������:
  //���������� ����������� ����� ���������� ������
  FrameEndCrc16((tU8*)FlashTmpBuffer, FlashTmpBufferSize);
  //����� ����� � ������       
  FlashDataProtectedWrite((tU32*)&FLASH_DATA, (tU32*)&BKFLASH_DATA);
}

void MemWrite::writeValueToCD(TInternalMemAddress& Addr, std::string& value){
  CopyFlashToTmpBuffer((tU32*)&CD_DATA);//���������� FALSH �� ��������� �����
  
  u32 v = std::stol(value, nullptr, 16);//��������
  u8* d = (u8*)&FlashTmpBuffer + Addr.Offset;//����� ����� �� ��������� ������
  
  if (Addr.BitNumber == -1) {
    memcpy(d, (u8*)&v, Addr.Lenght);//���������� ����� �� ��������� �����
  } else {//���� ������ Offset, BitNumber � Lenght
    u8 data = *d;//� ��� ���� ������ ������� �������� ����� �� ��������� �� ��� ������ ����� ����
    data = (v != 0) ? (data | (1<< Addr.BitNumber)) : ( data & ~(1 << Addr.BitNumber));
    memcpy(d, &data, Addr.Lenght);//�� � ����� ���� ������� ����� �� ��������� �����
  }
  //������ ��������� �� ��������� ������, ������:
  //���������� ����������� ����� ���������� ������
  FrameEndCrc16((tU8*)FlashTmpBuffer, FlashTmpBufferSize);
  //����� ����� � ������       
  __disable_irq(); // handles nested interrupt
  FlashSectorWrite((tU32*)&CD_DATA, (tU32*) FlashTmpBuffer);
  __enable_irq(); // handles nested interrupt
}
