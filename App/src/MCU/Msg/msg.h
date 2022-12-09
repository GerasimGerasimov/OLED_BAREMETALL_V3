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
	None    = 0, //ни одна клавиша не нажата
	ESC     = 8, //отмена (и выход на один уровень меню вверх)
	Up      = 16, //2 //стрелка вверх

	Right   = 256, //стрелка вправо
	ENT     = 32,//5 //выбор сделан
	Left    = 64, //стрелка влево
	
	Down    = 128,//8 //стрелка вниз
	F1      = 1, //функциональная клавиша F1
        F2      = 2, //функциональная клавиша F2
        F3      = 4 //функциональная клавиша F3
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