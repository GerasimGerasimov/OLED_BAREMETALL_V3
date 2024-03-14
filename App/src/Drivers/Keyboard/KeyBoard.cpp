#include "KeyBoard.h"
#include "msg.h"
#include "RAMdata.h"

static const u8 BTN_CNT_MAX = 10; //1 = мс сколько раз нужно проверить нажатие кнопки для подавления дребезга контактов

enum class KeyCtrl : u32 {
        ROW1 = GPIO_Pin_0,
        ROW2 = GPIO_Pin_1,
        ROW3 = GPIO_Pin_2,
        COL1 = GPIO_Pin_13,
        COL2 = GPIO_Pin_14,
        COL3 = GPIO_Pin_15,
        COLUMNS = 0xe000,
        COLUMNS_OFS = 0x07, 
        ROWS = 0x07
};

enum class KeyRepeatCtrl {
  First,
  Next
};

u16 KeyBoard::ScanCode = 0;
u16 KeyBoard::repeatState = (u16)KeyRepeatCtrl::First;
u16 KeyBoard::repeatDelay = 0;

void KeyBoard::sendKeyboardMsg(void) {
  static u16 prevScanCode = 0;
    if (ScanCode != prevScanCode) {
      Msg::send_message((u32)EventSrc::KEYBOARD, ScanCode, (u32)KeyPressFeature::FirstPress);
      prevScanCode = ScanCode;
      prepareToRepeat();
    } else {
      if (ScanCode) {
        repeating();
      }
    }
}

static const u16 FirstDelay = 650;
static const u16 NextDelay = 150;

void KeyBoard::prepareToRepeat(void){
  repeatState = (u16)KeyRepeatCtrl::First;
  repeatDelay = FirstDelay;
}

void KeyBoard::repeating(void){
  switch (repeatState) {
    case (u16)KeyRepeatCtrl::First:
        (repeatDelay)
          ? repeatDelay --
          : (repeatState = (u16)KeyRepeatCtrl::Next);
      break;
    case (u16)KeyRepeatCtrl::Next:
        (repeatDelay)
          ? repeatDelay --
          : ( repeatDelay = NextDelay,
             Msg::send_message((u32)EventSrc::KEYBOARD, ScanCode, (u32)KeyPressFeature::AutoRepeat),
                 0);    
    break;
  }
}

u16 KeyBoard::getScanCode(void) //функция проверки - нажата ли кнопка и если да - то какая
{ 
  u16 delay_cnt;
  u8 row1=0, row2=0,row3=0;// активная строка
  u16 col=0;// активный столбец
  u16 btn=0;// номер нажатой кнопки
  
    GPIOC->ODR ^= (u32)KeyCtrl::ROW1; //  подать 0 на первую строку
    delay_cnt = 0xff; //подождали
    while(delay_cnt--);
    col= GPIOB->IDR & (u32)KeyCtrl::COLUMNS;    //  считать данные столбцов GPIOC
    row1 = ((~(col>>13)) & (u32)KeyCtrl::COLUMNS_OFS); // получить код кнопок первой строки 3
    GPIOC->ODR |= (u32)KeyCtrl::ROWS; // отключить все строки
    
    GPIOC->ODR ^= (u32)KeyCtrl::ROW2; //  подать 0 на вторую строку
    delay_cnt = 0xff; //подождали
    while(delay_cnt--);
    col= GPIOB->IDR & (u32)KeyCtrl::COLUMNS;    //  считать данные столбцов GPIOC
    row2 = ((~(col>>13)) & (u32)KeyCtrl::COLUMNS_OFS); // получить код кнопок второй строки 3
    GPIOC->ODR |= (u32)KeyCtrl::ROWS; // отключить все строки
    
    GPIOC->ODR ^= (u32)KeyCtrl::ROW3; //  подать 0 на третью строку
    delay_cnt = 0xff; //подождали
    while(delay_cnt--);
    col= GPIOB->IDR & (u32)KeyCtrl::COLUMNS;    //  считать данные столбцов GPIOC
    row3 = ((~(col>>13)) & (u32)KeyCtrl::COLUMNS_OFS); // получить код кнопок третьей строки 3
    
    GPIOC->ODR |= (u32)KeyCtrl::ROWS; // отключить все строки

    btn = row1 + (row2 << 3) + (row3 << 6);

    return btn; // возврат номера нажатой кнопки
}


/*функция получения кода нажатой кнопки с подавлением дребезга*/
void KeyBoard::debouncing(u16 rawcode)
{
  static u16 btn_cur=0;
  static u16 btn_last=0;
  static u8 btn_cnt=0;
  
  if (rawcode ==0)ScanCode = 0; //нет кнопки - отправляем ноль один раз и молчим
  
  if (btn_cnt == 0) //начало отсчета проверки дребезга
  {
    if (rawcode != btn_cur) //пришла нажатая кнопка, первый раз
    {
      btn_cnt++; //начали отсчет проверки на дребезг
      btn_cur = rawcode; //сохранили пришедшее значение нажатой кнопки
    }    
  }
  else //проверка на дребезг уже идет
  {
    if (rawcode==btn_cur) btn_cnt++; //все еще нажата та кнопка
    else 
    {
      btn_cnt=0; //кнопка не нажата - сбрасываем проверку на дребезг     
    }
  }
  
  if(btn_cnt > BTN_CNT_MAX) //если проверили на дребезг сколько-то милисекунд
  {
    btn_cnt = 1; //сбросили счет в 1, тк кнопка еще не отжата
    if (btn_cur == btn_last) //если кнопка нажата та же, что и предыдущая
    {
      
        ScanCode = btn_cur; //в режиме сканкода пока кнопка нажата - ее отправл¤ем посто¤нно
    }
    else //если предыдущя нажатая кнопка была другой
    { 
      //if (ScanCode==0) //если не было нажатой кнопки 
      //{
        
        ScanCode = btn_cur; //1<<(btn_cur -1);  //говорим, что нажата кнопка скан код такой-то
        btn_last = btn_cur; //сохраняем ее значение 
      //}
      //else btn_last=0; //сохраняем в предыдущее значение = 0
    }
  }
  RAM_DATA.var2 = ScanCode;
}



