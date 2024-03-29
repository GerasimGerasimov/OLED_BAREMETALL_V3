#ifndef FLASHDATA_H
#define FLASHDATA_H
#include "Intmash_bastypes.h"//�������� �������� ������� �����
//��������� ������������ FLASH
typedef union _tOverturnXY {
	tU16 I;
        struct {
	tU8 X;//�������
        tU8 Y;//�������
        } Coord;

} tOverturnXY;

typedef struct {
    
    tU32 Modbus_U1_RS485;//0   ����� � �������� ��� ������ � ��������  
    tU32 Modbus_U2_RS485;//1 ����� � �������� ��� ������ � �������   
    tU16 TIME_OUT_Lnkmngr; //������� ��� ����� ������������� - ����� ���� ����, 10=1�� /5000
    tU16 TIME_CHANGE_Lnkmngr; //����� ����� ������ 10=1�� /500
    float Koef_null;
    float Koef[20];
    tU16 Password;
    tU16 Retranslate;
    tOverturnXY Overturn;
    tU16 __free__;
 } TFLASH_DATA;

//����������� ��������� ��������� ���������
#define FLASH_DATA_PREFIX 0x2
#define FLASH_DATA_SIZE 0xfe

//��������� ������������ FLASH
extern const TFLASH_DATA FLASH_DATA;
extern const TFLASH_DATA BKFLASH_DATA;

#endif 