#ifndef _SCREEN_DATA_H
#define _SCREEN_DATA_H

extern u8 data_client_RAM[256];
extern u8 data_client_FLASH[256];
extern u8 data_client_CD[256];



//тип структур экранов
/*
  name - имя экрана (страницы)
  number - номер страницы
  page_size - число используемых элементов на экране, максимально 99
  page_content[99] - массив элементов экрана, используется из них только page_size количество
  99 - номер элемента на экране
options - опциальная переменная, на всякий случай
  page_content[99] = адрес структуры в массиве элементов (0x0***) или в массиве экранов (0x1***)
*/
typedef struct
{
  char *name;
  u16 page_size; 
  u16 number;
  u16 page_content[99]; 
} page_type;

//тип структур элементов 
/*
  name - имя элемента
  comment - комментарий/справка
  measure - единицы измерения
  addres - адрес элемента - рам/флеш/сд различаются
  type - тип элемента
    type.b[0] - младший байт - сам тип переменной номер от 0 до 13
    type.b[1] - старший байт - подтип переменной (дли бита - инверсность, для остальных - размер переменной)
    0х1* и 0х0* - для бита инверсность, для остальных знаковое или беззнаковое
    0х*0 - бит, 0 байт, 0х*1 - 1 байт, 0х*2 - 2байта и тд
  koef - указатель на коэффициент из FLASH_DATA пример &FLASH_DATA.koef1
  offset_mask - бит-маска, лист - № в списке массивов лист, смещение для тех элементов, где нужно
  digits_after - число цифр после запятой, лист - количество используемых элементов в списке (максимально 10)  
  options - изменяемая переменная 1 или нет 0
    b[0]изменяемая переменная 1 или нет0
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
  u8 options; // b[0]изменяемая переменная 1 или нет0/ b[1] - тут номер num_koef в массиве коэффициентов Koef_Ram
} element_type;

#define CHANGE_PARAM_YES 1 //options, переменную можно изменять
#define CHANGE_PARAM_NO 0 //options, переменную нельзя изменять


//тип для параметров PrmList
/*
  code - код/значение переменной лист
  string - что обозначает это значение
*/
typedef struct
{
  u32 code;
  char *string;
} note;




extern element_type Elements[200];
extern element_type Password_Element; //для пароля
extern page_type Pages[];
extern note Lists[][10];
extern u16 alarms[32];
extern u16 warnings[32];


#endif // 