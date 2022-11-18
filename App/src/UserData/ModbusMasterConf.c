#include "STM32F4xx_Intmash_USART.h"
#include "ModbusSlave.h"
#include "crc16.h"
#include "RAMdata.h"
#include "flashdata.h"
#include "ModbusMasterConf.h"
//переменный для дисплея
#include "ModbusSlaveConf.h"

tU8 Flash_read=1;
tU8 CD_read=1;
tU16 scan_code=0;
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
MBmasterSlotType slot_FLASH_READ;
MBmasterSlotType slot_CD_READ;
MBmasterSlotType slot_ID_READ;
MBmasterSlotType slot_WRITE;
MBmasterSlotType slot_SEND_SCANCODE;
MBmasterSlotType slot_LOG_READ;

MBmasterSlotType slot_RTR; //для ретрансляции
tU16 RTR_size =0;
tU16 RTR_addr =0;
tU8 fRtrWaitResp =0;

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
void AVKWaitAnswer (void){
  UsartRecieve(&UARTtoAVK, CommonMaster.InBuf);  
}

void RAM_READ_Load (void)
{ 
  if (slot_RAM_READ.Flags.Bits.CRC_ERR || slot_RAM_READ.Flags.Bits.TIMEOUT_ERR) {
    //resetRamDataValid();
  } 	
  UsartTransmit(&UARTtoAVK, slot_RAM_READ.OutBuf, 8);//пакет уже подготовлен в буфере, просто отправляем его черезм ДМА и нужную шину
}

void RAM_READ_Finish (void)
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
//FLASH AVK
void FLASH_Load (void)
{
  UsartTransmit(&UARTtoAVK, slot_FLASH_READ.OutBuf, 8);//пакет уже подготовлен в буфере, просто отправляем его черезм ДМА и нужную шину
}
void FLASH_Finish (void)
{
  //надо свапнуть принятые данные
  //ModbusSwapCopy(&CommonMaster.InBuf[MB_DATA_SECTION_CMD_03],(u16 *) &data_client_FLASH[0], CommonMaster.InBuf[MB_DATA_BYTE_NUM_CMD_03] >> 1);
  //ScrDraw_update = 1;
}
//CD
void CD_Load (void)
{
 UsartTransmit(&UARTtoAVK, slot_CD_READ.OutBuf, 8);//пакет уже подготовлен в буфере, просто отправляем его черезм ДМА и нужную шину
}
void CD_Finish (void)
{
  //надо свапнуть принятые данные
  //ModbusSwapCopy(&CommonMaster.InBuf[MB_DATA_SECTION_CMD_03],(u16 *) &data_client_CD[0], CommonMaster.InBuf[MB_DATA_BYTE_NUM_CMD_03] >> 1);
  //ScrDraw_update = 1;
}
//ID
void ID_Load (void)
{
 UsartTransmit(&UARTtoAVK, slot_ID_READ.OutBuf, 4);//пакет уже подготовлен в буфере, просто отправляем его черезм ДМА и нужную шину
}

#define ID_MAX_LENGHT 200
void ID_Finish (void)
{
    u8 i=0;
    u16 leightID;
    leightID = CommonMaster.InBuf[2];
    if (leightID > ID_MAX_LENGHT) leightID = ID_MAX_LENGHT;//нефиг делать ID таким большим, оно ограничится 70
    for  (i=0;i < (leightID-1); i++ )
    {       
       DeviceID_[i] = CommonMaster.InBuf[i+3];
    } 
}

//WRITE 
void WRITE_Load (void) 
{   
  tU8 msg_size;
  
  slot_WRITE.OutBuf[4] = 0x00;                //num_reg_Hi;
  slot_WRITE.OutBuf[5] = write_num_byte>>1;   //num_reg_Lo; 
  slot_WRITE.OutBuf[6] = write_num_byte;        //количество передаваемых байт  
  msg_size = write_num_byte;//сколько передаем байт для записи
  //теперь считаем КС отправляемого пакета
  msg_size += 9;//размер пакета + контрольной суммы
  FrameEndCrc16(slot_WRITE.OutBuf, msg_size);   
  UsartTransmit(&UARTtoAVK, slot_WRITE.OutBuf, msg_size);//пакет подготовлен в буфере, просто отправляем его черезм ДМА и нужную шину
}

void WRITE_Finish (void)
{
  if (Flash_read)slot_FLASH_READ.Flags.Bits.SKIP_SLOT = 0; //считали для начала и все
  if (CD_read)slot_CD_READ.Flags.Bits.SKIP_SLOT = 0; 
}

//Сканкод 
void SCANCODE_Load (void)
{
  //btn_scan_code = 0; //в этой прошивке                         
  tU8 msg_size;
  msg_size = 2;//сколько передаем байт для записи

  //что собственно пишем то   - скан код из глобальной переменной
  slot_SEND_SCANCODE.OutBuf[7+0] = (u8)((scan_code>>8)&0x00ff);
  slot_SEND_SCANCODE.OutBuf[7+1] = (u8)(scan_code&0x00ff);  
  //теперь считаем КС отправляемого пакета
  msg_size +=9;//размер пакета без контрольной суммы
  FrameEndCrc16(slot_SEND_SCANCODE.OutBuf, msg_size); 
  UsartTransmit(&UARTtoAVK, slot_SEND_SCANCODE.OutBuf, msg_size);//пакет уже подготовлен в буфере, просто отправляем его черезм ДМА и нужную шину
}

void SCANCODE_Finish (void)
{
  
}

//Логирование 
void LOG_Load (void)
{
  /*
  if (TypeLogScr == POWER) slot_LOG_READ.OutBuf[1] = COM_POWERLOG;
  else if(TypeLogScr == BLACKBOX) slot_LOG_READ.OutBuf[1] = COM_BLACKBOX;
 
  slot_LOG_READ.OutBuf[3]= NumLogStr;
  FrameEndCrc16(slot_LOG_READ.OutBuf, 6); 
  UsartTransmit(&UARTtoAVK, slot_LOG_READ.OutBuf, 6);//пакет уже подготовлен в буфере, просто отправляем его черезм ДМА и нужную шину
  */
}

void LOG_Finish (void)
{
  /*
    tU8 i=0;
    for  (i=0;i < 250; i++ ) //на всякий случай обнулим все
    {
         logfile[i] = 0;  
    }
    
    for  (i=0;i < 200; i++ ) //тут отследить не будет ли ошибки!!!если будет строка длинной 254 байта
    {
         logfile[i] = CommonMaster.InBuf[i+2];  
    } 
    //защита от дурака
    if ((logfile[0] =='к')&(logfile[1] =='о'))
    {
      if ((logfile[2] =='н')&(logfile[3] =='е')) Flag_none=1; //дурак зашел туда, где ничего нет
      else Flag_none=0;
    }
    else Flag_none=0;
    //карты нет - что делать - сбросить номер запрашиваемой строки в 0
     if ((logfile[0] =='S')&(logfile[1] =='D')&(logfile[2] ==' ')&(logfile[3] =='к')) NumLogStr=0;
  return;
  */
}
/*
//ram_thcnt
void THCNTWaitAnswer (void){
  UsartRecieve(&UARTtoTHcnt, CommonMaster.InBuf);  
}

void THCNT_READ_Load (void)
{  
  UsartTransmit(&UARTtoTHcnt, slot_THCNT_READ.OutBuf, 8);//пакет уже подготовлен в буфере, просто отправляем его черезм ДМА и нужную шину
}
void THCNT_READ_Finish (void)
{
  // надо свапнуть   
  //ModbusSwapCopy(&CommonMaster.InBuf[MB_DATA_SECTION_CMD03_11],(u16 *) &data_client_RAM[200], CommonMaster.InBuf[MB_DATA_BYTE_NUM_CMD03_11] >> 1);
  data_client_RAM[200] = CommonMaster.InBuf[MB_DATA_SECTION_CMD_03 +1];
  data_client_RAM[201] = CommonMaster.InBuf[MB_DATA_SECTION_CMD_03 ];
  data_client_RAM[202] = CommonMaster.InBuf[MB_DATA_SECTION_CMD_03 +3];
  data_client_RAM[203] = CommonMaster.InBuf[MB_DATA_SECTION_CMD_03 +2];
  data_client_RAM[204] = CommonMaster.InBuf[MB_DATA_SECTION_CMD_03 +5];
  data_client_RAM[205] = CommonMaster.InBuf[MB_DATA_SECTION_CMD_03 +4];
  
  data_client_RAM[206] = CommonMaster.InBuf[MB_DATA_SECTION_CMD_03 +7];
  data_client_RAM[207] = CommonMaster.InBuf[MB_DATA_SECTION_CMD_03 +6];
  data_client_RAM[208] = CommonMaster.InBuf[MB_DATA_SECTION_CMD_03 +9];
  data_client_RAM[209] = CommonMaster.InBuf[MB_DATA_SECTION_CMD_03 +8];
  data_client_RAM[210] = CommonMaster.InBuf[MB_DATA_SECTION_CMD_03 +11];
  data_client_RAM[211] = CommonMaster.InBuf[MB_DATA_SECTION_CMD_03 +10];
  data_client_RAM[212] = CommonMaster.InBuf[MB_DATA_SECTION_CMD_03 +13];
  data_client_RAM[213] = CommonMaster.InBuf[MB_DATA_SECTION_CMD_03 +12];
  data_client_RAM[228] = CommonMaster.InBuf[MB_DATA_SECTION_CMD_03 +29];
  data_client_RAM[229] = CommonMaster.InBuf[MB_DATA_SECTION_CMD_03 +28];
  
  
  ScrDraw_update=1; //обновить экран - пришли новые данные
}

void THCNT_WRITE_Load (void) 
{ 
  tU8 msg_size = write_num_byte;//сколько передаем байт для записи
  
  slot_THCNT_WRITE.OutBuf[2] = 0x00;
  slot_THCNT_WRITE.OutBuf[3] = 0x16; //это уже регистр
  slot_THCNT_WRITE.OutBuf[4] = 0x00;                //num_reg_Hi;
  slot_THCNT_WRITE.OutBuf[5] = write_num_byte>>1;   //num_reg_Lo; 
  slot_THCNT_WRITE.OutBuf[6] = write_num_byte;        //количество передаваемых байт
 
  //теперь считаем КС отправляемого пакета
  msg_size += 9;//размер пакета + контрольной суммы
  FrameEndCrc16(slot_THCNT_WRITE.OutBuf, msg_size);   
  UsartTransmit(&UARTtoTHcnt, slot_THCNT_WRITE.OutBuf, msg_size);//пакет подготовлен в буфере, просто отправляем его черезм ДМА и нужную шину
}
void THCNT_WRITE_Finish (void)
{
  if (LED_RUN_ST) LED_RUN_ON; //помигали светиком связи
    else LED_RUN_OFF;
}

*/

//Ретрансляция
void RTR_Load (void)
{  
  RTR_addr = slot_RTR.OutBuf[0]; //сохранили адрес
  slot_RTR.OutBuf[0] = (tU8)(FLASH_DATA.Modbus_U2_RS485 & 0x000000ff);
  FrameEndCrc16(slot_RTR.OutBuf, RTR_size); //пересчитали crc
  UsartTransmit(&UARTtoAVK, slot_RTR.OutBuf, RTR_size);//пакет уже подготовлен в буфере, просто отправляем его черезм ДМА и нужную шину
}
void RTR_Finish (void)
{ // if (LED1_ST) LED1_OFF; else LED1_ON; //моргнули светиком 
  fRtrWaitResp =0;
  for(tU8 i=0; i<RTR_size;i++) {RS485slave.Buffer[i] = CommonMaster.InBuf[i];}
  RS485slave.Buffer[0] = RTR_addr; //вернули адрес
  FrameEndCrc16(RS485slave.Buffer, RTR_size); //пересчитали crc
  UsartTransmit(&UART1toRS485, &RS485slave.Buffer[0], RTR_size);
}

//инициализация мастера
void ModbusMasterInit(void){
  tU32Union InterfaceSettings;
  InterfaceSettings.I = FLASH_DATA.Modbus_U2_RS485;
  //RAM AVK
  slot_RAM_READ.Flags.I=0;
  slot_RAM_READ.Flags.Bits.REPEATING_TRANSMIT=1;
  slot_RAM_READ.OnStart=&RAM_READ_Load;
  slot_RAM_READ.ToRecieve=&AVKWaitAnswer;
  slot_RAM_READ.OnFinish=&RAM_READ_Finish;
  slot_RAM_READ.OutBuf[MB_DEVICE_ADDRESS]=InterfaceSettings.B[0];
  slot_RAM_READ.OutBuf[MB_COMMAND_CODE]=COM_READ;
  slot_RAM_READ.OutBuf[MB_START_ADDR_HI]=0x00;
  slot_RAM_READ.OutBuf[MB_START_ADDR_LO]=(tU8)RAM_START;
  slot_RAM_READ.OutBuf[MB_REG_NUM_HI_CMD_03]=0x00;
  slot_RAM_READ.OutBuf[MB_REG_NUM_LO_CMD_03]=RAM_NUM;
  FrameEndCrc16(slot_RAM_READ.OutBuf, 8);  
  //FLASH 
  slot_FLASH_READ.Flags.I=0;
  slot_FLASH_READ.Flags.Bits.SKIP_SLOT=1;
  slot_FLASH_READ.OnStart=&FLASH_Load;
  slot_FLASH_READ.ToRecieve=&AVKWaitAnswer;
  slot_FLASH_READ.OnFinish=&FLASH_Finish;
  slot_FLASH_READ.OutBuf[MB_DEVICE_ADDRESS]=InterfaceSettings.B[0];
  slot_FLASH_READ.OutBuf[MB_COMMAND_CODE]=COM_READ;
  slot_FLASH_READ.OutBuf[MB_START_ADDR_HI]=(tU8)(FLASH_START>>8);
  slot_FLASH_READ.OutBuf[MB_START_ADDR_LO]=(tU8)FLASH_START;
  slot_FLASH_READ.OutBuf[MB_REG_NUM_HI_CMD_03]=0x00;
  slot_FLASH_READ.OutBuf[MB_REG_NUM_LO_CMD_03]=FLASH_NUM;
  FrameEndCrc16(slot_FLASH_READ.OutBuf, 8);
  //CD 
  slot_CD_READ.Flags.I=0;
  slot_CD_READ.Flags.Bits.SKIP_SLOT=1;
  slot_CD_READ.OnStart=&CD_Load;
  slot_CD_READ.ToRecieve=&AVKWaitAnswer;
  slot_CD_READ.OnFinish=&CD_Finish;
  slot_CD_READ.OutBuf[MB_DEVICE_ADDRESS]=InterfaceSettings.B[0];
  slot_CD_READ.OutBuf[MB_COMMAND_CODE]=COM_READ;
  slot_CD_READ.OutBuf[MB_START_ADDR_HI]=(tU8)(CD_START>>8);
  slot_CD_READ.OutBuf[MB_START_ADDR_LO]=(tU8)CD_START;
  slot_CD_READ.OutBuf[MB_REG_NUM_HI_CMD_03]=0x00;
  slot_CD_READ.OutBuf[MB_REG_NUM_LO_CMD_03]=CD_NUM;
  FrameEndCrc16(slot_CD_READ.OutBuf, 8);  
  //ID 
  slot_ID_READ.Flags.I=0;
  slot_ID_READ.Flags.Bits.SKIP_SLOT=1;
  slot_ID_READ.OnStart=&ID_Load;
  slot_ID_READ.ToRecieve=&AVKWaitAnswer;
  slot_ID_READ.OnFinish=&ID_Finish;
  slot_ID_READ.OutBuf[MB_DEVICE_ADDRESS]=InterfaceSettings.B[0];
  slot_ID_READ.OutBuf[MB_COMMAND_CODE]=COM_ID;
  FrameEndCrc16(slot_ID_READ.OutBuf, 4);
  //Write 
  slot_WRITE.Flags.I=0;
  slot_WRITE.Flags.Bits.SKIP_SLOT=1;
  slot_WRITE.OnStart=&WRITE_Load;
  slot_WRITE.ToRecieve=&AVKWaitAnswer;
  slot_WRITE.OnFinish=&WRITE_Finish;
  slot_WRITE.OutBuf[MB_DEVICE_ADDRESS]=InterfaceSettings.B[0];
  slot_WRITE.OutBuf[MB_COMMAND_CODE]=COM_WRITE;
  //Сканкод
  slot_SEND_SCANCODE.Flags.I=0;
  slot_SEND_SCANCODE.Flags.Bits.SKIP_SLOT=1;
  slot_SEND_SCANCODE.OnStart=&SCANCODE_Load;
  slot_SEND_SCANCODE.ToRecieve=&AVKWaitAnswer;
  slot_SEND_SCANCODE.OnFinish=&SCANCODE_Finish;
  slot_SEND_SCANCODE.OutBuf[MB_DEVICE_ADDRESS]=InterfaceSettings.B[0];
  slot_SEND_SCANCODE.OutBuf[MB_COMMAND_CODE]=COM_SCANCODE;
  slot_SEND_SCANCODE.OutBuf[MB_START_ADDR_HI]=(tU8)(SCANCODE_ADDR>>8);
  slot_SEND_SCANCODE.OutBuf[MB_START_ADDR_LO]=(tU8)SCANCODE_ADDR;
  slot_SEND_SCANCODE.OutBuf[MB_REG_NUM_HI_CMD_10]=0x00;
  slot_SEND_SCANCODE.OutBuf[MB_REG_NUM_LO_CMD_10]=SCANCODE_NUM;
  FrameEndCrc16(slot_SEND_SCANCODE.OutBuf, 8);  
  //Логирование
  slot_LOG_READ.Flags.I=0;
  slot_LOG_READ.Flags.Bits.SKIP_SLOT=1;
  slot_LOG_READ.OnStart=&LOG_Load;
  slot_LOG_READ.ToRecieve=&AVKWaitAnswer;
  slot_LOG_READ.OnFinish=&LOG_Finish;
  slot_LOG_READ.OutBuf[MB_DEVICE_ADDRESS]=InterfaceSettings.B[0];
  slot_LOG_READ.OutBuf[MB_COMMAND_CODE]=COM_BLACKBOX;
  slot_LOG_READ.OutBuf[MB_START_ADDR_HI]= 0x00;
  slot_LOG_READ.OutBuf[MB_START_ADDR_LO]=0;//(tU8)NumLogStr;
  FrameEndCrc16(slot_LOG_READ.OutBuf, 6);  
  //ретрансляция
  slot_RTR.Flags.I=0;
  slot_RTR.Flags.I=0;
  slot_RTR.Flags.Bits.SKIP_SLOT=1;
  slot_RTR.OnStart=&RTR_Load;
  slot_RTR.ToRecieve=&AVKWaitAnswer;
  slot_RTR.OnFinish=&RTR_Finish;
  
    
  CommonMaster.CurrentSlot=0;
  CommonMaster.LastError=0;
  CommonMaster.Pause=20;
  CommonMaster.TimeOut=500;
  CommonMaster.Status=SELECTION;
  CommonMaster.SetTimer=&SetMasterTimer;
  CommonMaster.StopTimer=&StopMasterTimer;
  CommonMaster.Slots[0]=&slot_RAM_READ;
  CommonMaster.Slots[1]=&slot_FLASH_READ;
  CommonMaster.Slots[2]=&slot_CD_READ;
  CommonMaster.Slots[3]=&slot_ID_READ;
  CommonMaster.Slots[4]=&slot_WRITE;
  CommonMaster.Slots[5]=&slot_SEND_SCANCODE;
  CommonMaster.Slots[6]=&slot_RTR;
  
  CommonMaster.Slots[7]=0;
    
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
    if ((CommonMaster.CurrentSlot == 6)&&(fRtrWaitResp !=0)) RTR_size = TransferStatus; //сохранить рестрансляцию
    
    if (CommonMaster.InBuf[1] == COM_WRITE)RAM_DATA.var1 = CommonMaster.CurrentSlot;
  }
  
  ModbusMasterTransferProcessing(&CommonMaster, TransferStatus);
    
}


//функция для сброса зависания
void RebootMBMaster(void)
{
  UsartTxRxFinish(&UARTtoAVK); //сбросить преывание и все выкл
  
}
