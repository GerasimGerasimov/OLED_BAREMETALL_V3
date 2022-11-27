#include "stm32f4xx.h"
#include "bastypes.h"
#include "ramdata.h"//���������� ���������� � ��������� RAM_DATA
#include "LEDs.h"

//��� Spi ���������
//extern u8 framebuffer[8][128];




/*type ��������� ��� ���� ���������� � ��������� ������*/
#define TBit 0x0000
#define TByte 0x0001
#define TShortInt 0x1002
#define TWORD 0x0003
#define TInteger 0x1004
#define TDWORD 0x0005
#define TLongInt 0x1006
#define TIPAddr 0x0007
#define TIntegral 0x0008
#define TFrequency 0x0009
#define TSoftTimer 0x000a
#define TInputFilter 0x000b
#define TPrmList 0x000c
#define TFloat 0x100d

#define SECURE 0x000f //������ ��� ������, ������ ��� ������
//������� � ������
#define SByte_1 0x0100
#define SByte_2 0x0200
#define SByte_4 0x0400
//����������� ��� ����
#define INVERSE 0x1000


//������������ ������� �� � � �� � ��� �������
#define MAX_END_X 127
#define MAX_END_Y 63


#define ADRR_MASK_SCR 0x1000// ����� ������ ������
#define ADRR_MASK_EL 0x0fff// ����� ������ �������� (� ��������� � ������), ������� ������� �������, ������� ���������� - �������� ��� ��� �����

#define TYPE1_MASK_EL 0x0f //����� ��������� ������ ���������� � ������, 0-���,1 1�,2-2����� � ��



//����������� ����� ������ ����������?
//����� ������ �������� ������ (�� ����� �������)


extern u8 ScrDraw_update;//���������� 1 = ���� ������������ �����
extern u16 btn_scan_code; //1 ������� - 2�����
extern u32 test_num; //�������� ����������
extern char DeviceID_[256]; //��� ������������� ������ ��

extern u8 Actual_Scr; //����������, ������������ - ����� ������ ����� �������� �� �������
//�� ��������:
#define COMMSCR 0 //common screen - �����
#define EDITSCR 1 //edit screen - ���������
#define HELPSCR 2 //help screen - �������
#define INFOSCR 3 //���� screen - ���������� �� ����������
#define IPADDR_SCR 4 //����� ����� ��� ������
#define IPADDR_EDITSCR 5 //�������������� ������
#define LOGSCR 6 //����� �����������

#define F3_BUTTON_ON 0xf //������ ������ F3 � ���� �������� ��������
#define F3_BUTTON_OFF 0 //������ ������ F3 � ���� �������� ��������


#define BLACKBOX 0
#define POWER 1
//����� ������� powerlog � blackbox
extern u8 PowerLogScrNum;
extern u8 BlackBoxScrNum;
extern u8 NumLogStr; //���������� ���������� ��� ������ ������, ������� ������ �����������
extern  u8 TypeLogScr; //���������� - ����� ��� ���������� ��� ����
extern char logfile[250]; //��� ������ ����
extern u8 Flag_none; //������ �� ������








