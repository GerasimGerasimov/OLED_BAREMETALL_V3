#include "ModbusMaster.h"
#include "crc16.h"

/*Обработка транзакции, вызывает в прерывании интерфеса связи*/
void ModbusMasterTransferProcessing(MBmasterType* Master, tU16 TransferStatus)
{  
  MBmasterSlotType *CurSlot;
  CurSlot = Master->Slots[Master->CurrentSlot];//получаем ссылку на текущий слот
  
  if (TransferStatus){//принято сообщение
    Master->StopTimer();//таймаута больше не ждем
    if(crc16(Master->InBuf, TransferStatus)){ //проверяем crc принятого ответа
      //последняя ошибка = номер слота в старшем байте и код ошибки сrc в младшей
      Master->LastError = (Master->CurrentSlot<<8) + CRC_MASK;
      CurSlot->Flags.Bits.CRC_ERR=1;//выставляем флаг у слота
    } 
    else {//сообщение успешно принятно и не повреждено! почистим флаги и обработаем ответ
      CurSlot->Flags.Bits.CRC_ERR=0;//сбрасываем флаги ошибок 
      CurSlot->Flags.Bits.TIMEOUT_ERR=0;  	  
      //если есть подпрограмма обработки принятого пакета - вызвать её    
      if (CurSlot->OnFinish) CurSlot->OnFinish();      
    }
    CurSlot->Flags.Bits.SKIP_SLOT=1;//слот выполнен - скипуем
    //пауза между транзакциями
    Master->Status=PAUSE;//пауза в работе мастера
    Master->SetTimer(Master->Pause);//установили таймер на паузу
  }
  
  
  else{//если сообщение было отправлено       
    if (CurSlot->ToRecieve){//если есть процедура после отправки 
      CurSlot->ToRecieve();//выполнили процедуру
      Master->Status=RECIEVE;//мастер ожидает прием сообщения
      Master->SetTimer(Master->TimeOut);//установили таймер на ожидание таймаута
    }
    else{//если ответа не требуется
      CurSlot->Flags.Bits.SKIP_SLOT=1;//слот выполнен - скипуем
      //пауза между транзакциями
      Master->Status=PAUSE;//пауза в работе мастера
      Master->SetTimer(Master->Pause);//установили таймер на паузу
    }
  }
  
}


/*Функция обработки мастера по истечению пауз таймера, вызывается в прерывании таймера*/
void ModbusMasterTimerProcessing (MBmasterType* Master)
{
   //если была пауза между транзакциями
   if(Master->Status==PAUSE){
     //необходимо выбрать следующий слот для обработки  
     Master->Status=SELECTION;   	   
   }
   
   //если ожидали приема
   else if(Master->Status==RECIEVE){
     //Очевидно, не долждались, таймаут сработал 
     MBmasterSlotType *CurSlot;
     CurSlot = Master->Slots[Master->CurrentSlot];//получаем ссылку на текущий слот
     //последняя ошибка = номер слота в старшем байте и код ошибки таймаута в младшей	 
     Master->LastError = (Master->CurrentSlot<<8) + TIMEOUT_MASK;
     CurSlot->Flags.Bits.TIMEOUT_ERR=1;//выставляем флаг у слота
     CurSlot->Flags.Bits.SKIP_SLOT=1;//слот выполнен - скипуем
     //необходимо выбрать следующий слот для обработки  
     Master->Status=SELECTION;
   }   
}

void ModbusMasterSlotSelection(MBmasterType* Master){
  MBmasterSlotType *CurSlot;
  
  if(Master->Status==SELECTION){		
    for (tU8 SlotIdx = 0; SlotIdx < MB_MASTER_SLOTS_MAX; SlotIdx++) {//перебираем все слоты         
      CurSlot = Master->Slots[SlotIdx];//получаем ссылку на слот
      if(CurSlot){//перебираем слоты, пока ссылки на слоты определены
        //если слот не скипуется - обрабатываем его   
	if (CurSlot->Flags.Bits.SKIP_SLOT == 0){
	  //теперь этот слот текущий
	  Master->CurrentSlot=SlotIdx;
	  if (CurSlot->OnStart) {
	    CurSlot->OnStart();//заряжаем его на отправку
            Master->Status=TRANSMIT;//ждем отправки 
            break; //если слот выбран - выходим
	  }				   
        } 
      } else break;//как только доходим до нулевого указателя - слоты кончились          
    } 
    //если перебрали все слоты, а активного не нашлось, то ищем повторяющиеся слоты
    if(Master->Status==SELECTION){
      for (tU8 SlotIdx = 0; SlotIdx < MB_MASTER_SLOTS_MAX; SlotIdx++){
        CurSlot = Master->Slots[SlotIdx];//получаем ссылку на слот
	if(CurSlot){//перебираем слоты, пока ссылки на слоты определены
	  //если слот повторяющийся - сбросили скип, сделали его активным
          if (CurSlot->Flags.Bits.REPEATING_TRANSMIT)	CurSlot->Flags.Bits.SKIP_SLOT=0;				
	} else break;//как только доходим до нулевого указателя - слоты кончились				
      } 
    }	
  }	
}


void ActivateSlot(MBmasterSlotType* Slot){
	Slot->Flags.Bits.SKIP_SLOT=0;
}






