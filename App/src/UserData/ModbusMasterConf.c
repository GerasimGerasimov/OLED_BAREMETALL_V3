#include "STM32F4xx_Intmash_USART.h"
#include "flashdata.h"
#include "ModbusMasterConf.h"
#include "STM32F4xx_Intmash_Flash.h"

MBmasterSlotType SlotMaster;

Intmash_Usart UART_MASTER;

static u16 TimeOut = 0;
static u8* ReplyPtr = 0;
//функции таймера
void SetMasterTimer(tU16 delay){  
  TIM7->CNT = 0;
  TIM7->ARR = delay*10;//зарядить на нужную паузу.
  TIM7->SR = 0; //снять флаг прерывания 
  TIM_Cmd(TIM7, ENABLE);//запустили снова, ждем когда пауза сработает.
}

void StopMasterTimer(void){ 
  TIM_Cmd(TIM7, DISABLE);//
}

//инициализация мастера
#define INTERFACE_MASTER_DEFAULT_SETTINGS (u32)(0x00000401)
void ModbusMasterInit(void){
  tU32Union InterfaceSettings;
  InterfaceSettings.I = FLASH_DATA.Modbus_U2_RS485;

  SlotMaster.OnTimeOut = 0;
  SlotMaster.OnRecieve = 0;
    
  //инициализация драйвер для общения с PWR (через кроссплату)
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 
  UART_MASTER.USARTx = USART2;
  UART_MASTER.USART_BaudRate = USARTbaudRate[InterfaceSettings.B[1]];
  UART_MASTER.USART_StopBits=InterfaceSettings.B[2];
  UART_MASTER.USART_Parity=InterfaceSettings.B[3];
  UART_MASTER.DMAy_StreamRX = DMA1_Stream5;
  UART_MASTER.DMAy_StreamTX = DMA1_Stream6;
  UART_MASTER.DMA_channel = DMA_Channel_4;
  UART_MASTER.DMA_FLAGS_RX = DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5 | DMA_FLAG_TCIF5;
  UART_MASTER.DMA_FLAGS_TX = DMA_FLAG_FEIF6 | DMA_FLAG_DMEIF6 | DMA_FLAG_TEIF6 | DMA_FLAG_HTIF6 | DMA_FLAG_TCIF6;  
  UART_MASTER.PolarityDIR = POLARITY_HI; //?
  UART_MASTER.GPIOxDIR = GPIOA;
  UART_MASTER.GPIO_PinDIR = GPIO_Pin_1;
  UsartDriverInit (&UART_MASTER); //инициализация периферии
       
  //таймер для мастера
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;      
  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;//частота = TIM4CLK(84МГц)/8400 = 10КГц.
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 0xffff;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM7, ENABLE);    
  TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM7, DISABLE);//таймер включается после отправки запроса линк менеджером
  TIM7->SR = 0;  
    
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   
  NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;//таймер общего назначения
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure); 
}

//прерывания
void TIM7_IRQHandler(void){
  TIM7->SR = 0;
  StopMasterTimer();
  //досчитал тайм аута 
  //TODO вызвать функцию парсинга с передачей ей статуса TIMEOUT
  //ModbusMasterTimerProcessing(&CommonMaster); 
  if (SlotMaster.OnTimeOut) {
      SlotMaster.OnTimeOut();
  }
}


void ModbusMasterSetCondition(u16 timeOut, u8* replyPtr) {
  TimeOut = timeOut;
  ReplyPtr = replyPtr;
}

void ModbusMasterSend(u8* data, u8 len){
  SlotMaster.InBufLen = 0;
  UsartTransmit(&UART_MASTER, data, len);
}

void USART2_IRQHandler(void){ 
  tU16 TransferStatus = UsartTxRxFinish(&UART_MASTER);
  //0 - данные были отправлены
  //!0 - что-то принято
  //если данные отправлены, то надо включить таймер таймаута для ожидания ответа
  if (TransferStatus == 0) {
    SetMasterTimer(TimeOut);//установили таймер на ожидание таймаута
    UsartRecieve(&UART_MASTER, ReplyPtr);//и ждать поступления данных
  } else {//какие-то данные приняты (в ответ на запрос)
    StopMasterTimer();
    if (SlotMaster.OnRecieve) {
      SlotMaster.InBufLen = TransferStatus;
      SlotMaster.OnRecieve();
    }
  }
}


//функция для сброса зависания
void RebootMBMaster(void)
{
  UsartTxRxFinish(&UART_MASTER); //сбросить преывание и все выкл
}


/*
void onFinish (void)
{
  //static u8 kostil=0;
  // надо свапнуть   
  //ModbusSwapCopy(&CommonMaster.InBuf[MB_DATA_SECTION_CMD_03],(u16 *) &data_client_RAM[0], CommonMaster.InBuf[MB_DATA_BYTE_NUM_CMD_03] >> 1);
  //ScrDraw_update=1; //обновить экран - пришли новые данные
  //setRamDataValidState();
  //nr читаем постоянно - тут можно проверять некоторые данные
  //if (LED_LINK_ST) LED_LINK_ON; //помигали светиком связи
  //else LED_LINK_OFF;
}
*/