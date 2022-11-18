#ifndef _SCREEN_DATA_H
#define _SCREEN_DATA_H

extern u8 data_client_RAM[256];
extern u8 data_client_FLASH[256];
extern u8 data_client_CD[256];



//��� �������� �������
/*
  name - ��� ������ (��������)
  number - ����� ��������
  page_size - ����� ������������ ��������� �� ������, ����������� 99
  page_content[99] - ������ ��������� ������, ������������ �� ��� ������ page_size ����������
  99 - ����� �������� �� ������
options - ���������� ����������, �� ������ ������
  page_content[99] = ����� ��������� � ������� ��������� (0x0***) ��� � ������� ������� (0x1***)
*/
typedef struct
{
  char *name;
  u16 page_size; 
  u16 number;
  u16 page_content[99]; 
} page_type;

//��� �������� ��������� 
/*
  name - ��� ��������
  comment - �����������/�������
  measure - ������� ���������
  addres - ����� �������� - ���/����/�� �����������
  type - ��� ��������
    type.b[0] - ������� ���� - ��� ��� ���������� ����� �� 0 �� 13
    type.b[1] - ������� ���� - ������ ���������� (��� ���� - �����������, ��� ��������� - ������ ����������)
    0�1* � 0�0* - ��� ���� �����������, ��� ��������� �������� ��� �����������
    0�*0 - ���, 0 ����, 0�*1 - 1 ����, 0�*2 - 2����� � ��
  koef - ��������� �� ����������� �� FLASH_DATA ������ &FLASH_DATA.koef1
  offset_mask - ���-�����, ���� - � � ������ �������� ����, �������� ��� ��� ���������, ��� �����
  digits_after - ����� ���� ����� �������, ���� - ���������� ������������ ��������� � ������ (����������� 10)  
  options - ���������� ���������� 1 ��� ��� 0
    b[0]���������� ���������� 1 ��� ���0
*/
typedef struct
{
  char *name;
  char *comment;
  char *measure;
  u16 addres;
  bavu16 type;
  const float *koef;
  u32 offset_mask;
  u8 digits_after;
  u8 options; // b[0]���������� ���������� 1 ��� ���0/ b[1] - ��� ����� num_koef � ������� ������������� Koef_Ram
} element_type;

#define CHANGE_PARAM_YES 1 //options, ���������� ����� ��������
#define CHANGE_PARAM_NO 0 //options, ���������� ������ ��������


//��� ��� ���������� PrmList
/*
  code - ���/�������� ���������� ����
  string - ��� ���������� ��� ��������
*/
typedef struct
{
  u32 code;
  char *string;
} note;




extern element_type Elements[200];
extern element_type Password_Element; //��� ������
extern page_type Pages[];
extern note Lists[][10];
extern u16 alarms[32];
extern u16 warnings[32];


#endif // 