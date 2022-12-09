#include "buttons.h"
#include "RAMdata.h"

#define BTN_CNT_MAX 10 //1 = �� ������� ��� ����� ��������� ������� ������ ��� ���������� �������� ���������

u16 btn_scan_code;


u16 buttons_check (void) //������� �������� - ������ �� ������ � ���� �� - �� �����
{ 
  u16 delay_cnt;
  u8 row1=0, row2=0,row3=0;// �������� ������
  u16 col=0;// �������� �������
  u16 btn=0;// ����� ������� ������
  
    GPIOC->ODR ^= ROW1; //  ������ 0 �� ������ ������
    delay_cnt = 0xff; //���������
    while(delay_cnt--);
    col= GPIOB->IDR & COLUMNS;    //  ������� ������ �������� GPIOC
    row1 = ((~(col>>13)) & COLUMNS_OFS); // �������� ��� ������ ������ ������ 3
    GPIOC->ODR |= ROWS; // ��������� ��� ������
    
    GPIOC->ODR ^= ROW2; //  ������ 0 �� ������ ������
    delay_cnt = 0xff; //���������
    while(delay_cnt--);
    col= GPIOB->IDR & COLUMNS;    //  ������� ������ �������� GPIOC
    row2 = ((~(col>>13)) & COLUMNS_OFS); // �������� ��� ������ ������ ������ 3
    GPIOC->ODR |= ROWS; // ��������� ��� ������
    
    GPIOC->ODR ^= ROW3; //  ������ 0 �� ������ ������
    delay_cnt = 0xff; //���������
    while(delay_cnt--);
    col= GPIOB->IDR & COLUMNS;    //  ������� ������ �������� GPIOC
    row3 = ((~(col>>13)) & COLUMNS_OFS); // �������� ��� ������ ������� ������ 3
    
    GPIOC->ODR |= ROWS; // ��������� ��� ������

    btn = row1 + (row2 << 3) + (row3 << 6);

    return btn; // ������� ������ ������� ������
}


/*������� ��������� ���� ������� ������ � ����������� ��������*/
void buttons_scan(u16 btn_tmp)
{
  static u16 btn_cur=0;
  static u16 btn_last=0;
  static u8 btn_cnt=0;
  
  if (btn_tmp ==0)btn_scan_code = 0; //��� ������ - ���������� ���� ���� ��� � ������
  
  if (btn_cnt == 0) //������ ������� �������� ��������
  {
    if (btn_tmp != btn_cur) //������ ������� ������, ������ ���
    {
      btn_cnt++; //������ ������ �������� �� �������
      btn_cur = btn_tmp; //��������� ��������� �������� ������� ������
    }    
  }
  else //�������� �� ������� ��� ����
  {
    if (btn_tmp==btn_cur) btn_cnt++; //��� ��� ������ �� ������
    else 
    {
      btn_cnt=0; //������ �� ������ - ���������� �������� �� �������     
    }
  }
  
  if(btn_cnt > BTN_CNT_MAX) //���� ��������� �� ������� �������-�� ����������
  {
    btn_cnt = 1; //�������� ���� � 1, �� ������ ��� �� ������
    if (btn_cur == btn_last) //���� ������ ������ �� ��, ��� � ����������
    {
        btn_scan_code = btn_cur; //� ������ �������� ���� ������ ������ - �� ��������� ��������
    }
    else //���� ��������� ������� ������ ���� ������
    { 
      if (btn_scan_code==0) //���� �� ���� ������� ������ 
      {
        btn_scan_code = btn_cur; //1<<(btn_cur -1);  //�������, ��� ������ ������ ���� ��� �����-��
        btn_last = btn_cur; //��������� �� �������� 
      }
      else btn_last=0; //��������� � ���������� �������� = 0
    }
  }
  RAM_DATA.var1 = btn_scan_code;
}



