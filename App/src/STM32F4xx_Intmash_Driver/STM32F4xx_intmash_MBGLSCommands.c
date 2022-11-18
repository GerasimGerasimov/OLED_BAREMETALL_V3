#include "STM32F4xx_intmash_MBGLSCommands.h"
#include "Intmash_bastypes.h"
#include "crc16.h"


/*
  ������� 0x25 0x26 0x27   ��� ������ � GLS-�������
  ������� �� STM32F4xx_intmash_MBfilesCommands
  ��� ��������� ���������� �� ������������, ������ ��� ���������� STM32F4xx_intmash_MBfilesCommands
*/


#define CRC_SIZE 2 //������ crc-2 �����
#define MB_HEAD_SIZE 2 //������ ��������� ������
#define DISK_ERROR 0x80 //������ ����� - ����������� ��� �� ���������������
#define FILE_ERROR 0xC0 //������ ����� - �����������, �� ������� �������
#define FAT_ERROR 0xC0 //������ �������� �������
#define FIND_NEXT_FILE 0xFF //�����  ������ ���������� �����, � �� ������ ������� �� SD 

//��� ������������� (� ���? ���������� �������� ��.)
char  GLSFileName [13];//��� ��������� GLS-���������
//�������� ������ ��������� ���������
void FS_ClearGLSFileName(void)
{
  tU8 c;
  //���� GLSFileName, �������� ������
  c = sizeof (GLSFileName);
  while (c != sizeof (GLSFileName)) 
  {
    c--;
    GLSFileName[c] = 0;
  }
}
tU16 FastDataPoints = 0; //����� �����, ������������ ����� �� ��� ����
/*  
  0�25 ��������� ��� ����� � ��������� �� ������� �����
    ���� �������� ������ ��� ���������� ����������� (�������� ������ ����� ���(����������)/����� ��� �������������� ���������)
    � ����� ������ ������� �������� �����, ������ ��� ���������, � ������ ��������� ������ �� ��������� �������
  ��������� 01.25.FILE#0.CRC
    �N - ���-�� ���� � ����� �����
    FILE - ��� ����� � ������� 8.3(������ ���� filename.rec)
  ����� 01.25.CRC
    ������/���������
    0x25 | 0x00 = 0010.0011 - �� �������
    0x25 | 0x80 = 1010.0011 - ���� �� ��������������� ��� �����������
    0x25 | 0x�0 = 1110.0011 - ���� �����������
*/
tU8 ModbusSetGLSFile(ModbusSlaveType* Slave)
{
  volatile FRESULT res;
  volatile UINT  br;   // ���-�� ����������� ���� �� �����
  volatile tU32Union offs; //�������� � �����
  tU8  c = 0;//������� ��� ����������� ����� �����
  tU8 DataLength = 2; //������ ������������ ������� - ����� ��������� ����+�������
  
  if (FSReady == FAT_READY)//�������� ������� ������, ����� ��������
  {
    res = f_open(&CurrentFileOperationsStruct->f, (char const*)&Slave->Buffer[2], FA_OPEN_EXISTING |FA_READ);// Open an existing file
    if (res == FR_OK) 
    {
      //�������� ��� ����� � GLSFileName
      while (c != sizeof (GLSFileName)) //���� �� ��������� � ���� ���� ���� ������� �� ��������� �� ����� ������
      {
        if ((GLSFileName[c] = Slave->Buffer[2+c]) == 0) break;
        c++;
      }
      //���� ������ � ������ �� ������
      //���������� ��������, ������� ������ ���� �� ����� ������� �����
      //���� � GLS ������ ���������� ���������� � "0"-��������, ����� �������� �������� 512 ���� �������� ���.��������      
      offs.I = 0;
      res = f_lseek(&CurrentFileOperationsStruct->f, offs.I); //�������� ������ �� "0"
      //������� ���-�� ����� ���������
      if (f_size(&CurrentFileOperationsStruct->f) > 512) FastDataPoints = (f_size(&CurrentFileOperationsStruct->f) - 512) >> 2;//������� �� 4 ������� ���-�� ����� � ���������
      else FastDataPoints = 0;//��� ����� � �� ���������� � �����
      //�� ���������� ���� ������ � ������ ���� ����, ������ � ���� ��� ������
      //� ������� � ����������� �� ������� �������
      //�������� ��������� ������ ���� ���������� �����������
     // RAM_DATA.Player  &= ~(fPLAY | fPAUSE);//���������� ��������������� PLAY = 0;
     // RAM_DATA.FastData.PlayPos = 0;//������ ��������� � ������
     // TECH_FILE_OFFS = GLS_BIN_DATA_OFFS;//��������� �������� � �����
     // TECH_INIT = TRUE;//������� ��� ���� ������ � �� � �������
    }
    else //���� �����������... �� ��� ������ ��������
    {    
      FS_ClearGLSFileName();
      Slave->Buffer[1] |= FILE_ERROR; //error code = �0
    }
  }
  else Slave->Buffer[1] |= DISK_ERROR; //error code = 80 //�������� ������� �� ������

  DataLength += CRC_SIZE;//��������� ����� crc
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;

}

/*
  0�26  ���������� ��� ��������� ���������������� �����
  ��������� 01.26.CRC
  ����� 01.26.FILE#.CRC
    FILE � ��� ����� � ������� 8.3(�0-������������� ������ ���� filename.rec)
  ������/���������
    01.A6.CRC 0x26 | 0x80 = 1010.0001 � ���� �� ��������������� ��� �����������
    01.26.00.CRC � ���� ������ ���� � ����� �����, ������ ����� ��� ��� �� ����� 
*/
tU8 ModbusGetGLSFile(ModbusSlaveType* Slave) //0x26 ���������� ��� ��������� �����
{ 
  tU8 CN = 0;
  tU8 DataLength = 2; //������ ������������ ������� - ����� ��������� ����+�������
  if (FSReady == FAT_READY) 
  {
    while ((Slave->Buffer[2+CN] = GLSFileName[CN]) !=0 ) //����������� ����� ������ � ����������� �����
    { 
      if (CN == sizeof (GLSFileName)) //������������ � ��������� ����� ������ �� 13 ��������(� �� � ���� ��� ��� ����)
      { 
        CN =0; //���� ���� �� ��������, �� ��� ����� � �� ������, ���� ��� ����� ���� � �� ����� �� �����
        Slave->Buffer[2+CN] = 0;//��� �����
        break;
      }
      CN++;
    }
    //������ � CN-������� ���� � ������ (����� ���� � 0)
    DataLength += 1 + CN; //�������� ������ ������ � ����������� ������
  }
  else Slave->Buffer[1] |= 0x80; //error code = 80 //�������� ������� �� ������ 
  
  DataLength += CRC_SIZE;//��������� ����� crc
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;
}


/*  0�27 ������ ��� ����� ��� �������� ������� �����
    ��������� 01.27.FILE#0.CRC
      �N - ���-�� ���� � ����� �����
      FILE - ��� ����� � ������� 8.3(������ ���� filename.rec)
  ����� 01.27.CRC
  ������/���������
    0x27 | 0x00 = 0010.0011 - �� �������
    0x27 | 0x80 = 1010.0011 - ���� �� ��������������� ��� �����������

*/
tU8 ModbusSetGLSFileName(ModbusSlaveType* Slave)
{
  volatile FRESULT res;
  volatile UINT  br;   // ���-�� ����������� ���� �� �����
  volatile tU32Union offs; //�������� � �����
  tU8  c = 0;//������� ��� ����������� ����� �����
  tU8 DataLength = 2; //������ ������������ ������� - ����� ��������� ����+�������
  
  if (FSReady == FAT_READY)//�������� ������� ������, ����� ��������
  {
      //�������� ��� ����� � GLSFileName ��� �������� �� ��������� ����-����
      while (c != sizeof (GLSFileName)) //���� �� ��������� � ���� ���� ���� ������� �� ��������� �� ����� ������
      {
        if ((GLSFileName[c] = Slave->Buffer[2+c]) == 0) break;
        c++;
      }
  }
  else Slave->Buffer[1] |= DISK_ERROR; //error code = 80 //�������� ������� �� ������

  DataLength += CRC_SIZE;//��������� ����� crc
  FrameEndCrc16((tU8*)Slave->Buffer, DataLength);
  return DataLength;

}

