#include "stm32f4xx.h"
#include "bastypes.h"
#include "ramdata.h"//глобальные переменные и структура RAM_DATA
#include "LEDs.h"

//для Spi необходим
//extern u8 framebuffer[8][128];




/*type константы для типа переменной в элементах экрана*/
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

#define SECURE 0x000f //особый тип данных, только для экрана
//размеры в байтах
#define SByte_1 0x0100
#define SByte_2 0x0200
#define SByte_4 0x0400
//инверсность для бита
#define INVERSE 0x1000


//максимальные позиции по х и по у для дисплея
#define MAX_END_X 127
#define MAX_END_Y 63


#define ADRR_MASK_SCR 0x1000// маска адреса экрана
#define ADRR_MASK_EL 0x0fff// маска адреса элемента (и параметра и экрана), убирает старшую тетраду, которая показывает - параметр это или экран

#define TYPE1_MASK_EL 0x0f //маска оставляет размер переменной в байтах, 0-бит,1 1б,2-2байта и тд



//задефайнили номер экрана Информации?
//номер экрана Настойки панели (из флеша дисплея)


extern u8 ScrDraw_update;//переменная 1 = надо перерисовать экран
extern u16 btn_scan_code; //1 регистр - 2байта
extern u32 test_num; //тестовая переменная
extern char DeviceID_[256]; //для линкменеджера чтения ид

extern u8 Actual_Scr; //переменная, отображающая - какой сейчас экран рисовать на дисплее
//ее значения:
#define COMMSCR 0 //common screen - общий
#define EDITSCR 1 //edit screen - изменения
#define HELPSCR 2 //help screen - справка
#define INFOSCR 3 //инфо screen - информация об устройстве
#define IPADDR_SCR 4 //экран чисто для адреса
#define IPADDR_EDITSCR 5 //редактирование адреса
#define LOGSCR 6 //экран логирования

#define F3_BUTTON_ON 0xf //нажата кнопка F3 и надо посылать сканкоды
#define F3_BUTTON_OFF 0 //нажата кнопка F3 и надо посылать сканкоды


#define BLACKBOX 0
#define POWER 1
//номер экранов powerlog и blackbox
extern u8 PowerLogScrNum;
extern u8 BlackBoxScrNum;
extern u8 NumLogStr; //глобальная переменная для номера строки, которую сейчас запрашиваем
extern  u8 TypeLogScr; //переменная - пишем лог можщностей или ящик
extern char logfile[250]; //для чтения лога
extern u8 Flag_none; //защита от дурака








