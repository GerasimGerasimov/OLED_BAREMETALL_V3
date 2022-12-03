#ifndef MODBUSMASTER_H
#define MODBUSMASTER_H
/*****************************************************************************************************
Данный модуль предоставляет средства для развертывания Modbus-мастера.

Modbus-мастер работает по следующему алгоритму:
1)ожидание выбора текущего исполняемого слота (статус мастера SELECTION). Слот выбирается среди тех, 
у кого не выставлен флаг пропуска слота (SKIP), если таких слотов не нашлось - флаги SKIP сбрасываются 
у слотов с выставленным флагом цикличной отправки (REPEATING_TRANSMIT);

2)исполняемый слот выбран, началась передача его запроса слейву (статус мастера TRANSMIT);

3)передача запроса слейву завершена, идет прием ответного сообщения, если оно необходимо (RECIEVE);

4)после приема ответного сообщения, либо если ответного сообщения ждать не нужно, либо если сработал
таймаут - слот считается выполненым, выставляется его флаг SKIP, для его пропуска при следующей фазе
выбора исполняемого слота, при этом мастер переходит в состояние паузы перед исполнением следующего 
слота (статус PAUSE).

5) всё начинается с пункта 1.   



Для работы мастера необходимо:
1) Создать структуру типа мастера (MBmasterType).

2) Создать структуры типа слотов мастера (MBmasterSlotType). Для каждой операции можно создать 
по отдельному слоту, но можно испльзовать один или несколько, изменяя их в течение работы. 

3) Инициализировать структуру мастера, необходимо задать:
- указатель на функцию, инициализирующую временную задержку (аргумент функции - задержка в мс);
- указатель на функцию, останавливающую временную задержку (отмена задержки, остановка счетчика);
- время таймаута в мс (ожидание ответа для всех слотов одинаковое);
- время паузы между обработками двух слотов в мс (для всех одинаковое);
- инициализировать нулем последнюю ошибку и текущий выполняемый слот;
- заполнить буфер указателей на обрабатываемые слоты, начиная с 0 индекса, последовательно, без 
пропусков индексов, элемент массива после последнего слота инициализировать нулем (обязательно).
- при необходимости очистить буфер приема слота (буфер ответного сообщения);

4) Инициализировать слоты для обработки операций (каждый слот):

 - Структура слота содержит буфер исходящего сообщения, его можно инициализировать после создания 
слота (если сообщение не будет меняться в течение работы слота, например, когда слот выполняет 
регулярную операцию чтения фиксированого набора регистров с устройства с фиксированным адресом);

При инициализации слота необходимо задать указатели функций обработчиков: 
- процедура, которая выполняет действия для отправки сообщения, например отправка сообщения (буфер
слота) через какой-либо интерфейс (так же можно перед отправкой собрать исходящее собщение, если 
есть необходимость).
- процедура, которая выполняет действия после отправки сообщения, например инициализация приема 
ответа каким-либо интерфейсом.
- процедура, которая выполняет действия после приема ответного (валидного), например обработка
принятых данных (преобразование).

- флаги слота либо инициализируется нулями, либо выставляется флаг SKIP (если нет необходимости
в выполнении слота сразу после начала работы мастера, а только по возникновению необходимости),
или флаг REPEATING_TRANSMIT(если после каждого выполнения слота необходимо повторить выполнения, 
например для регулярного чтения данных с другого устройства).

5) все указатели на функции, упомянутые выше должны существовать и быть работоспособными.

6) Разместить в коде функции управления мастером. 
void ModbusMasterSlotSelection(MBmasterType* Master); - функция выполняющая выбор слота для исполнения
(выполняется, если статус мастера SELECTION) и отправка его запроса слейву, если активный слот был 
найден. Слоты перебираются и выполняются по очереди. Обычно вызывается в main. 
  
void ModbusMasterTransferProcessing(MBmasterType* Master, tU16 TransferStatus); - функция, вызывается
после отправки запроса (TransferStatus = 0) или приема ответа от слейва(TransferStatus = размер 
принятого сообщения в байтах). После отправки переключает мастер на прием (если не указана процедура
после отправки - слот считается выполненым), после приема проверяет ответ на валидность, выставляет 
ошибку или вызывает процедуру обработки ответа. Обычно вызывается в прерывании интерфейса (Тх, Rx).
 
- void ModbusMasterTimerProcessing (MBmasterType* Master); - функция вызывается в прерывании таймера, 
обрабатывающего временные задержки. В зависимости от текущего статуса мастера (PAUSE или RECIEVE) 
либо выставляет статус SELECTION, либо выставляет флаг таймаута у нужного слота.

- void ActivateSlot(MBmasterSlotType* Slot); - функция сбрасывает флаг SKIP у указанного слота (чтобы
он мог быть выбран при выборе мастером слотов для исполнения), вызывается где угодно, по необходимости.

Для запуска работы мастера необходимо его статус инициализировать значением SELECTION.

Пример конфигурации указан в коментариях в конце данного файла.
******************************************************************************************************/

#include "Intmash_bastypes.h"


#define MB_MASTER_BUF_SIZE  256
#define MB_MASTER_SLOTS_MAX 1

//слот для организации типовой транзакции
typedef struct {  
  //буфер сообщения для отправки   
  tU8 OutBuf [MB_MASTER_BUF_SIZE]; 
  tU8 InBuf [MB_MASTER_BUF_SIZE]; 
  //процедура которая вызывается если данные не получены
  void (*OnTimeOut)(void);
  //что делаем после получения каких-то данных
  void (*OnRecieve)(void);
  //флаги (ошибки, служебные флаги)
  tU8Union Flags;
  u8 InBufLen;
} MBmasterSlotType;


typedef struct {  
  //буфер приемника   
  tU8 InBuf[MB_MASTER_BUF_SIZE]; 
  //буфер указателей на слоты, обрабатываемые данным мастером
  MBmasterSlotType* Slots[MB_MASTER_SLOTS_MAX];
  //процедура которая включает таймер на нужную задержку (в ms)
  void (*SetTimer)(tU16 Time);
  //процедура остановки таймера
  void (*StopTimer)(void);
  //временные настройки мастера (в ms)
  tU16 TimeOut;//таймаут
  tU16 Pause;//пауза между двумя транзакциями
  //текущий выполняемый слот
  tU16 CurrentSlot;
  //последняя возникшая ошибка
  tU16 LastError;
  //Статус работы мастера (что делает сейчас)   
  tU16  Status;
} MBmasterType;
//Статусы работы мастера
#define SELECTION 1
#define TRANSMIT  2
#define RECIEVE   4
#define PAUSE     8
//маски ошибок в LastError
#define TIMEOUT_MASK 1
#define CRC_MASK     2

  #ifdef __cplusplus
    extern "C" void ModbusMasterTimerProcessing (MBmasterType* Master);
    extern "C" void ModbusMasterSlotSelection(MBmasterType* Master); 
    extern "C" void ModbusMasterTransferProcessing(MBmasterType* Master, tU16 TransferStatus);
    extern "C" void ActivateSlot(MBmasterSlotType* Slot);
  #else
    extern void ModbusMasterTimerProcessing (MBmasterType* Master);
    extern void ModbusMasterSlotSelection(MBmasterType* Master); 
    extern void ModbusMasterTransferProcessing(MBmasterType* Master, tU16 TransferStatus);
    extern void ActivateSlot(MBmasterSlotType* Slot);
  #endif

/***Пример конфигурации*********************************************************************************
//структуры мастера и слотов
MBmasterType CommonMaster;
MBmasterSlotType PwrRead;



//таймер мастера: установка таймера, остановка таймера, прерывание.
void SetMasterTimer(tU16 delay)
{  
  TIM7->CNT = 0;
  TIM7->ARR = delay*10;//зарядить на нужную паузу.
  TIM7->SR = 0; //снять флаг прерывания 
  TIM_Cmd(TIM7, ENABLE);//запустили снова, ждем когда пауза сработает.
}
void StopMasterTimer(void)
{ 
  TIM_Cmd(TIM7, DISABLE);
}
void TIM7_IRQHandler(void)
{
  TIM7->SR = 0;
  StopMasterTimer();
  ModbusMasterTimerProcessing(&CommonMaster); 
}


//интерфейс мастера: структура драйвера, прерывания
Intmash_Usart UARTtoCRB;
void USART6_IRQHandler(void)
{  
  ModbusMasterTransferProcessing(&CommonMaster, UsartTxRxFinish(&UARTtoCRB));
}

//функции процедур слота PwrRead
void PwrReadLoad (void)//загрузка слота на отправку 
{
  UsartTransmit(&UARTtoCRB, PwrRead.OutBuf, 8); //пакет уже подготовлен в буфере, просто отправляем его черезм ДМА и нужную шину 
}
void PwrReadWaitAnswer (void)//переводим слот в ожидание ответа
{
  UsartRecieve(&UARTtoCRB, CommonMaster.InBuf); //настраиваем слот на приём сообщения 
}
void PwrReadFinish (void)//обработка ответа 
{  
  tU16 SwapTmp;  
  tU8  SwapIdx=MB_DATA_SECTION_CMD_03;  
  
  ModbusSwapCopy((tU8*)&CommonMaster.InBuf[SwapIdx], &SwapTmp, 1);
  fRi = ((float)SwapTmp);  
  SwapIdx+=2;
  
  ModbusSwapCopy((tU8*)&CommonMaster.InBuf[SwapIdx], &SwapTmp, 1);
  fXi = ((float)SwapTmp);    
}


//Инициализация мастера и интерфейсов
void ModbusMasterInit(void)
{
  CommonMaster.CurrentSlot=0;
  CommonMaster.LastError=0;
  CommonMaster.Pause=20;
  CommonMaster.TimeOut=500;
  CommonMaster.SetTimer=&SetMasterTimer;
  CommonMaster.StopTimer=&StopMasterTimer;
  CommonMaster.Slots[0]=&PwrRead;
  CommonMaster.Slots[1]=0;
  
  PwrRead.Flags.I=0;
  PwrRead.Flags.Bits.REPEATING_TRANSMIT=1;
  PwrRead.OnStart=&PwrReadLoad;
  PwrRead.ToRecieve=&PwrReadWaitAnswer;
  PwrRead.OnFinish=&PwrReadFinish;
  PwrRead.OutBuf[MB_DEVICE_ADDRESS]=0x01;
  PwrRead.OutBuf[MB_COMMAND_CODE]=0x03;
  PwrRead.OutBuf[MB_START_ADDR_HI]=0x00;
  PwrRead.OutBuf[MB_START_ADDR_LO]=0x05;
  PwrRead.OutBuf[MB_REG_NUM_HI_CMD_03]=0x00;
  PwrRead.OutBuf[MB_REG_NUM_LO_CMD_03]=0x02;
  //теперь считаем КС отправляемого пакета
  FrameEndCrc16(PwrRead.OutBuf, READ_CMD_SIZE);
  
  //инициализация драйвер UASRT
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE); 
  UARTtoCRB.USARTx = USART6;
  UARTtoCRB.USART_BaudRate = 115200;
  UARTtoCRB.DMAy_StreamRX = DMA2_Stream1;
  UARTtoCRB.DMAy_StreamTX = DMA2_Stream6;
  UARTtoCRB.DMA_channel = DMA_Channel_5;
  UARTtoCRB.DMA_FLAGS_RX = DMA_FLAG_FEIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1 | DMA_FLAG_TCIF1;
  UARTtoCRB.DMA_FLAGS_TX = DMA_FLAG_FEIF6 | DMA_FLAG_DMEIF6 | DMA_FLAG_TEIF6 | DMA_FLAG_HTIF6 | DMA_FLAG_TCIF6;  
  UARTtoCRB.PolarityDIR = WITHOUT_DIR; //дира нет
  UsartDriverInit (&UARTtoCRB); //инициализация периферии  
      
  //таймер для мастера
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
  TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;//частота = TIM4CLK(84МГц)/8400 = 10КГц.
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 0xffff;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM7, DISABLE);    
  TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM7, DISABLE);//таймер включается после отправки запроса линк менеджером
  TIM7->SR = 0;  
    
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   
  NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;//таймер общего назначения
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure); 
}
******************************************************************************************************/


#endif
