#include "STM32F4xx_Intmash_Flash.h"
#include "stm32f4xx.h"
#include "crc16.h"


tU8 FlashTmpBuffer[FlashTmpBufferSize];
tU8Union FlashStatus; //������ FLASH � BKFLASH 

/*Name: FlashSectorWrite
	������ ������ �� ������ � ������ Flash
Arg:
	tU32* FlashSectorAddr - ��������� �� ������ �������
        tU32* Buffer - ���������� �� ������ ������, ��������� ������
Ret:
Comment:
*/
void FlashSectorWrite(tU32* FlashSectorAddr, tU32* Buffer)
{
  volatile FLASH_Status FLASHStatus_;
  tU32 Count = FlashTmpBufferSize_dw;

  if (FlashStatus.Bits.FLASH_WRITE_DIS) return; //���� ��������� ��� ������� - ������ ������, �������

  __disable_irq(); // handles nested interrupt
  FLASH_Unlock();  // Unlock the Flash Program Erase controller
  /* Clear All pending flags */
  FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
  switch((tU32)FlashSectorAddr)
  {
    case 0x08000000: // SECTOR0_START_ADDR
      FLASHStatus_ = FLASH_EraseSector(FLASH_Sector_0, VoltageRange_3);
      break;
    case 0x08004000: // SECTOR1_START_ADDR
      FLASHStatus_ = FLASH_EraseSector(FLASH_Sector_1, VoltageRange_3);
      break;
    case 0x08008000: // SECTOR2_START_ADDR
      FLASHStatus_ = FLASH_EraseSector(FLASH_Sector_2, VoltageRange_3);
      break;
    case 0x0800C000: // SECTOR3_START_ADDR
      FLASHStatus_ = FLASH_EraseSector(FLASH_Sector_3, VoltageRange_3);
      break;
      case 0x08010000: // SECTOR4_START_ADDR
      FLASHStatus_ = FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3);
      break;
    case 0x08020000: // SECTOR5_START_ADDR
      FLASHStatus_ = FLASH_EraseSector(FLASH_Sector_5, VoltageRange_3);
      break;
    case 0x08040000: // SECTOR6_START_ADDR
      FLASHStatus_ = FLASH_EraseSector(FLASH_Sector_6, VoltageRange_3);
      break;
    case 0x08060000: // SECTOR7_START_ADDR
      FLASHStatus_ = FLASH_EraseSector(FLASH_Sector_7, VoltageRange_3);
      break;   
    case 0x08080000: // SECTOR8_START_ADDR
      FLASHStatus_ = FLASH_EraseSector(FLASH_Sector_8, VoltageRange_3);
      break;
    case 0x080A0000: // SECTOR9_START_ADDR
      FLASHStatus_ = FLASH_EraseSector(FLASH_Sector_9, VoltageRange_3);
      break;
    case 0x080C0000: // SECTOR10_START_ADDR
      FLASHStatus_ = FLASH_EraseSector(FLASH_Sector_10, VoltageRange_3);
      break;
    case 0x080E0000: // SECTOR11_START_ADDR
      FLASHStatus_ = FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3);
      break;
  }

  while(Count !=0 )
  {
    FLASHStatus_ = FLASH_ProgramWord((tU32)FlashSectorAddr, *Buffer);
    FlashSectorAddr ++;
    Buffer ++;
    Count --;
  }
  FLASH_Lock();
  __enable_irq(); // handles nested interrupt
  FlashStatus.Bits.FLASH_CHANGE = 1; //��������� ��� �� ��������� ������
}

/*Name: CopyFlashToTmpBuffer
	�������� ���������� ��������� ������� ���� ������ �� ��������� ����� (�������� �������)
Arg:
	tU32* Addr - ��������� �� ������ ���������
Ret:
Comment:
*/
void CopyFlashToTmpBuffer(tU32* Addr){
  tU32 *dest   = (tU32 *) FlashTmpBuffer;
  tU32  count  = FlashTmpBufferSize_dw;
  while (count != 0) {//21 �������
    *dest++ = *Addr++;
    count--;
  };
}

/*Name: FlashDataProtectedWrite
	���������� ������ � ������ BKFLASH � ������ FLASH, ��������� ������������ ������. 
        ��� ������ ������ ���������� ����� ������
Arg:
	tU32* FlashDataSector - ��������� yf ������ ������� FLASH
        tU32* BackupDataSector - ���������� �� ������ ������� BKFLASH
Ret:
Comment:
*/
void FlashDataProtectedWrite(tU32* FlashDataSector, tU32* BackupDataSector)
{
  volatile tU16 crc, _crc;
  
  //���� ������ ��������� ������� - �� �����, ���� ���������, ����� ����������� �� ���������.
  if (FlashStatus.Bits.FLASH_DATA_ERR) return;
  
  FlashSectorWrite(BackupDataSector, (tU32*)FlashTmpBuffer);//�������� �����
  if (crc16((tU8*)BackupDataSector, FlashTmpBufferSize)==0)
  {
    //��������� ������������ ������ � �����, ���� ������� ��� - ����� �������� ������
       FlashSectorWrite(FlashDataSector, (tU32*)FlashTmpBuffer);
       FlashStatus.Bits.BACKUP_DATA_ERR = 0; 
       if (crc16((tU8*)FlashDataSector, FlashTmpBufferSize)==0) FlashStatus.Bits.FLASH_DATA_ERR = 0; //���� ��� ������ - �������� ����
       else FlashStatus.Bits.FLASH_DATA_ERR =1;
  }
  else 
  {
        //���� �������� ������, ��������� ������ ����� ��������.
        FlashStatus.Bits.BACKUP_DATA_ERR = 1; 
  }
  
}


/*Name: CheckFlashData
	�������� �������� BKFLASH � FLASH. ���� ���� ������ ����� - ���������� ������� 
        ������������ ��� �� ������� �������. ���� ��� �� ������� - ����������� ���� ������.
        ���� ��� ������� ����� - ������������ ����� ������
Arg:
	tU32* FlashDataSector - ��������� yf ������ ������� FLASH
        tU32* BackupDataSector - ���������� �� ������ ������� BKFLASH
Ret:
Comment:
*/
void CheckFlashData(tU32* FlashDataSector, tU32* BackupDataSector)
{
  //������� ��������� ����������� ������� �� ����
  tU8 *crcl1;
  tU8 *crcl2;
  tU8 *crch1;
  tU8 *crch2;
  
  FlashStatus.Bits.BACKUP_DATA_ERR = 0;
  FlashStatus.Bits.FLASH_DATA_ERR = 0;
  if (crc16((tU8*)FlashDataSector, FlashTmpBufferSize))
  {    
    //����������� ����� �� �������, ������ ����� ��� ����� ��������
    if (crc16((tU8*)BackupDataSector, FlashTmpBufferSize)) 
    {
      FlashStatus.Bits.FLASH_DATA_ERR = 1; //��� ������� ����� ��� ����� ��������
      FlashStatus.Bits.BACKUP_DATA_ERR = 1; 
    }
    else  
    {
      FlashSectorWrite(FlashDataSector, BackupDataSector);//�������� ������������ �� ������
      if (crc16((tU8*)FlashDataSector, FlashTmpBufferSize)) FlashStatus.Bits.FLASH_DATA_ERR  = 1; //�� ����������, ������ ������
    }    
  }
  else
  {
     //���������� ��������, ������ � �������
    if (crc16((tU8*)BackupDataSector, FlashTmpBufferSize)) 
    {
      FlashSectorWrite(BackupDataSector, FlashDataSector);//��������� ������ �����, �������� ������������
      if (crc16((tU8*)BackupDataSector, FlashTmpBufferSize)) FlashStatus.Bits.BACKUP_DATA_ERR = 1; //�� ����������       
    }
    else {//��� ������� � �������
      crcl1 = (tU8*)FlashDataSector + (FlashTmpBufferSize-2);
      crcl2 = (tU8*)BackupDataSector + (FlashTmpBufferSize-2);
      crch1 = (tU8*)FlashDataSector + (FlashTmpBufferSize-1);
      crch2 = (tU8*)BackupDataSector + (FlashTmpBufferSize-1);
      if ((*crcl1 != *crcl2)||(*crch1 != *crch2)){//���� ����������� ����� � �������� ������
        FlashSectorWrite(BackupDataSector, FlashDataSector);//������� � ����� �������� ������
        if (crc16((tU8*)BackupDataSector, FlashTmpBufferSize)) FlashStatus.Bits.BACKUP_DATA_ERR = 1; //�� ����������
      }
    }
  }
}


