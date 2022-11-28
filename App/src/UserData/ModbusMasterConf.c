#include "STM32F4xx_Intmash_USART.h"
#include "ModbusSlave.h"
#include "crc16.h"
#include "RAMdata.h"
#include "flashdata.h"
#include "ModbusMasterConf.h"
//переменный для дисплея
#include "ModbusSlaveConf.h"

tU8 write_num_byte = 0; //переменная для записи. сколько байт надо записать в устройство, 2 или 4

char DeviceID_[256];
//настройки слотов
#define CLIENT_ADDR 1
#define RAM_START   0x0000
#define RAM_NUM     0x0058 //200
#define WRITE_NUM   0x01
#define FLASH_START 0x2000
#define FLASH_NUM   0x0043 
#define CD_START    0xc000
#define CD_NUM      0x0024 

#define SCANCODE_NUM 0x01
#define SCANCODE_ADDR 0x000a//регистровый адрес

#define COM_READ    0x03
#define COM_WRITE   0x10
#define COM_ID      0x11  
#define COM_SCANCODE 0xFE
#define COM_BLACKBOX 0x31 
#define COM_POWERLOG 0x32

MBmasterSlotType slot_RAM_READ;

MBmasterType CommonMaster;
Intmash_Usart UARTtoAVK;

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

//функции слота чтения RAM AVK
void onWaitAnswer (void){
  UsartRecieve(&UARTtoAVK, CommonMaster.InBuf);  
}

void onStart (void)
{ 
  if (slot_RAM_READ.Flags.Bits.CRC_ERR || slot_RAM_READ.Flags.Bits.TIMEOUT_ERR) {
    //resetRamDataValid();
  } 	
  UsartTransmit(&UARTtoAVK, slot_RAM_READ.OutBuf, 8);//пакет уже подготовлен в буфере, просто отправляем его черезм ДМА и нужную шину
}

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

//инициализация мастера
void ModbusMasterInit(void){
  tU32Union InterfaceSettings;
  InterfaceSettings.I = FLASH_DATA.Modbus_U2_RS485;
  //RAM AVK
  slot_RAM_READ.Flags.I=0;
  slot_RAM_READ.Flags.Bits.SKIP_SLOT = 1;
  slot_RAM_READ.OnStart=&onStart;
  slot_RAM_READ.ToRecieve=&onWaitAnswer;
  slot_RAM_READ.OnFinish=&onFinish;
  //slot_RAM_READ.OutBuf[MB_DEVICE_ADDRESS]=InterfaceSettings.B[0];
  //slot_RAM_READ.OutBuf[MB_COMMAND_CODE]=COM_READ;
  //slot_RAM_READ.OutBuf[MB_START_ADDR_HI]=0x00;
  //slot_RAM_READ.OutBuf[MB_START_ADDR_LO]=(tU8)RAM_START;
  //slot_RAM_READ.OutBuf[MB_REG_NUM_HI_CMD_03]=0x00;
  //slot_RAM_READ.OutBuf[MB_REG_NUM_LO_CMD_03]=RAM_NUM;
  //FrameEndCrc16(slot_RAM_READ.OutBuf, 8);  
    
  CommonMaster.CurrentSlot=0;
  CommonMaster.LastError=0;
  CommonMaster.Pause=20;
  CommonMaster.TimeOut=500;
  CommonMaster.Status=SELECTION;
  CommonMaster.SetTimer=&SetMasterTimer;
  CommonMaster.StopTimer=&StopMasterTimer;
  CommonMaster.Slots[0]=&slot_RAM_READ;
    
  //инициализация драйвер для общения с PWR (через кроссплату)
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 
  UARTtoAVK.USARTx = USART2;
  UARTtoAVK.USART_BaudRate = USARTbaudRate[InterfaceSettings.B[1]];
  UARTtoAVK.USART_StopBits=InterfaceSettings.B[2];
  UARTtoAVK.USART_Parity=InterfaceSettings.B[3];
  UARTtoAVK.DMAy_StreamRX = DMA1_Stream5;
  UARTtoAVK.DMAy_StreamTX = DMA1_Stream6;
  UARTtoAVK.DMA_channel = DMA_Channel_4;
  UARTtoAVK.DMA_FLAGS_RX = DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5 | DMA_FLAG_TCIF5;
  UARTtoAVK.DMA_FLAGS_TX = DMA_FLAG_FEIF6 | DMA_FLAG_DMEIF6 | DMA_FLAG_TEIF6 | DMA_FLAG_HTIF6 | DMA_FLAG_TCIF6;  
  UARTtoAVK.PolarityDIR = POLARITY_HI; //?
  UARTtoAVK.GPIOxDIR = GPIOA;
  UARTtoAVK.GPIO_PinDIR = GPIO_Pin_1;
  UsartDriverInit (&UARTtoAVK); //инициализация периферии
      
 /* RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//
  UARTtoTHcnt.USARTx = USART1;
  UARTtoTHcnt.USART_BaudRate = USARTbaudRate[FLASH_DATA.Modbus_U1_RS485.B[1]];
  UARTtoTHcnt.DMAy_StreamRX = DMA2_Stream5;
  UARTtoTHcnt.DMAy_StreamTX = DMA2_Stream7;
  UARTtoTHcnt.DMA_channel = DMA_Channel_4;
  UARTtoTHcnt.DMA_FLAGS_RX = DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5 | DMA_FLAG_TCIF5;
  UARTtoTHcnt.DMA_FLAGS_TX = DMA_FLAG_FEIF7 | DMA_FLAG_DMEIF7 | DMA_FLAG_TEIF7 | DMA_FLAG_HTIF7 | DMA_FLAG_TCIF7;  
  UARTtoTHcnt.PolarityDIR = POLARITY_HI; //?
  UARTtoTHcnt.GPIOxDIR = GPIOA;
  UARTtoTHcnt.GPIO_PinDIR = GPIO_Pin_8;
  UsartDriverInit (&UARTtoTHcnt); //инициализация периферии
  */
  
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
  TIM_ARRPreloadConfig(TIM7, DISABLE);    
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
  ModbusMasterTimerProcessing(&CommonMaster); 
}

tU16 RetrLinkCnt=0; //переменная для сброса остутствия связи
void USART2_IRQHandler(void){ 
  
  tU16 TransferStatus = UsartTxRxFinish(&UARTtoAVK);
   RetrLinkCnt =0; //
  if (TransferStatus){  
    if (CommonMaster.InBuf[1] == COM_WRITE)RAM_DATA.var1 = CommonMaster.CurrentSlot;
  }
  
  ModbusMasterTransferProcessing(&CommonMaster, TransferStatus);
    
}


//функция для сброса зависания
void RebootMBMaster(void)
{
  UsartTxRxFinish(&UARTtoAVK); //сбросить преывание и все выкл
}
