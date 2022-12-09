#include "buttons.h"
#include "RAMdata.h"

#define BTN_CNT_MAX 10 //1 = мс сколько раз нужно проверить нажатие кнопки дл€ подавлени€ дребезга контактов

u16 btn_scan_code;


u16 buttons_check (void) //функци€ проверки - нажата ли кнопка и если да - то кака€
{ 
  u16 delay_cnt;
  u8 row1=0, row2=0,row3=0;// активна€ строка
  u16 col=0;// активный столбец
  u16 btn=0;// номер нажатой кнопки
  
    GPIOC->ODR ^= ROW1; //  подать 0 на первую строку
    delay_cnt = 0xff; //подождали
    while(delay_cnt--);
    col= GPIOB->IDR & COLUMNS;    //  считать данные столбцов GPIOC
    row1 = ((~(col>>13)) & COLUMNS_OFS); // получить код кнопок первой строки 3
    GPIOC->ODR |= ROWS; // отключить все строки
    
    GPIOC->ODR ^= ROW2; //  подать 0 на вторую строку
    delay_cnt = 0xff; //подождали
    while(delay_cnt--);
    col= GPIOB->IDR & COLUMNS;    //  считать данные столбцов GPIOC
    row2 = ((~(col>>13)) & COLUMNS_OFS); // получить код кнопок второй строки 3
    GPIOC->ODR |= ROWS; // отключить все строки
    
    GPIOC->ODR ^= ROW3; //  подать 0 на третью строку
    delay_cnt = 0xff; //подождали
    while(delay_cnt--);
    col= GPIOB->IDR & COLUMNS;    //  считать данные столбцов GPIOC
    row3 = ((~(col>>13)) & COLUMNS_OFS); // получить код кнопок третьей строки 3
    
    GPIOC->ODR |= ROWS; // отключить все строки

    btn = row1 + (row2 << 3) + (row3 << 6);

    return btn; // возврат номера нажатой кнопки
}


/*функци€ получени€ кода нажатой кнопки с подавлением дребезга*/
void buttons_scan(u16 btn_tmp)
{
  static u16 btn_cur=0;
  static u16 btn_last=0;
  static u8 btn_cnt=0;
  
  if (btn_tmp ==0)btn_scan_code = 0; //нет кнопки - отправл€ем ноль один раз и молчим
  
  if (btn_cnt == 0) //начало отсчета проверки дребезга
  {
    if (btn_tmp != btn_cur) //пришла нажата€ кнопка, первый раз
    {
      btn_cnt++; //начали отсчет проверки на дребезг
      btn_cur = btn_tmp; //сохранили пришедшее значение нажатой кнопки
    }    
  }
  else //проверка на дребезг уже идет
  {
    if (btn_tmp==btn_cur) btn_cnt++; //все еще нажата та кнопка
    else 
    {
      btn_cnt=0; //кнопка не нажата - сбрасываем проверку на дребезг     
    }
  }
  
  if(btn_cnt > BTN_CNT_MAX) //если проверили на дребезг сколько-то милисекунд
  {
    btn_cnt = 1; //сбросили счет в 1, тк кнопка еще не отжата
    if (btn_cur == btn_last) //если кнопка нажата та же, что и предыдуща€
    {
        btn_scan_code = btn_cur; //в режиме сканкода пока кнопка нажата - ее отправл§ем посто§нно
    }
    else //если предыдущ€ нажата€ кнопка была другой
    { 
      if (btn_scan_code==0) //если не было нажатой кнопки 
      {
        btn_scan_code = btn_cur; //1<<(btn_cur -1);  //говорим, что нажата кнопка скан код такой-то
        btn_last = btn_cur; //сохран€ем ее значение 
      }
      else btn_last=0; //сохран€ем в предыдущее значение = 0
    }
  }
  RAM_DATA.var1 = btn_scan_code;
}



