//служба сообщений
#ifndef MSG_H
#define MSG_H

#include "stm32f4xx.h"

//defines
#define MAXMESSAGE  0xFFFF


enum class EventSrc {
	NONE,
	KEYBOARD,
	REPAINT,
	TIMER,
	DATAUPDATE
};

//Клавиатура
/*коды клавиатуры
   P |  UP   | ESC
--------------------
LEFT | ENTER | RIGHT
--------------------
  F1 | DOWN  |   F2
*/

enum class KeyCodes : u32 {
	None    = 0, //ниодна клавиша не нажата
	ESC     = 1, //отмена (и выход на один уровень меню вверх)
	Up      = 72, //2 //стрелка вверх
	P       = 3, //вызов меню
	Right   = 4, //стрелка вправо
	ENT     = 13,//5 //выбор сделан
	Left    = 6, //стрелка влево
	F2      = 7, //функциональная клавиша F2
	Down    = 80,//8 //стрелка вниз
	F1      = 9 //функциональная клавиша F1
};

typedef struct {//угу, вдруг мышь подключу ;-)
  u16 x;
  u16 y;
  u8  Key;
} TMouseState;

typedef struct {//структура сообщения
  u32 Event;//тип сообщения
  //параметры сообщения
  u32 p1;//первый параметр
  u32 p2;//второй параметр
} TMessage;

#define msg_buff_size 32 //размер буфера сообщений

class Msg {
	public:
		static void send_message(u32 event, u32 p1, u32 p2);//добавить сообщение в конец очереди
		static bool get_message(TMessage* m);//извлечь первое в очереди сообщение
		static void clear_msg_queue();//очистка очереди сообщений
	private:
		static TMessage Messages[msg_buff_size];//очередь сообщений
};

#endif