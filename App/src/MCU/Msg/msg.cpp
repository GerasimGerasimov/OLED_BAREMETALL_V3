#include "msg.h"

TMessage Msg::Messages[msg_buff_size];//очередь сообщений

u32 msg_queue_end = 0;//конец очереди
u32 msg_queue_beg = 0;//начало очереди

//void key_board();//отслеживает нажатия клавишь клавиатуры
//key_board отслеживает нажатия клавишь клавиатуры
//текущее состояние отличается от текущего, добавляет в очередь сообщений
//сообщение о нажатии клавиши и её код
/*
void key_board (){
  static u8 key_pred = 0;
  u8 key_tmp = buttons_check ();
  if (key_tmp != key_pred) {//работаю, если новый код не совпадает
     key_pred = key_tmp;
     //передаю сообщение, что изменилось состояние клавиатуры
     //нажата такая-то клавиша.
     send_message(KEYBOARD, key_tmp, 0);
  }
}
*/

void Msg::clear_msg_queue(){//очистка очереди сообщений
  u32 i = msg_buff_size;
  msg_queue_end = 0;//конец очереди
  msg_queue_beg = 0;//начало очереди
  while (i != 0) {//заполняю нулями все поля буфера
    i--;
    Messages[i].Event = 0;
    Messages[i].p1 = 0;
    Messages[i].p2 = 0;  
  }
}

//очередь сообщений
//функция send_message, ставит новое сообщение в конец очереди
//при этом конец очереди сдвигатеся на 1
//и если достиг максимального значения (кол-во сообщений в буфере)
//перескакиваю в "0 значение" - в общем если мессаги из очереди не извлекаются
//они по кругу затираются более свежими сообщениями
void Msg::send_message(u32 event, u32 p1, u32 p2){
  Messages[msg_queue_end].Event = event;
  Messages[msg_queue_end].p1 = p1;
  Messages[msg_queue_end].p2 = p2;
  msg_queue_end++;
  if (msg_queue_end >= msg_buff_size) {msg_queue_end = 0;}
}

bool Msg::get_message(TMessage* m){
  if (msg_queue_beg == msg_queue_end) {return false;};//очередь пуста, если указатели начала и конца равны
  *m = Messages[msg_queue_beg]; //копирую содержимое первого в очережи сообщения
  msg_queue_beg++; //назначаю следующего
  if (msg_queue_beg >= msg_buff_size) {msg_queue_beg = 0;}//контролирую границы
  return true;//в очереди что-то было!
}