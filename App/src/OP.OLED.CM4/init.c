#include "stm32f4xx.h" 
#include "ramdata.h"
#include "flashdata.h"
#include "crc16.h"
#include "ssd1305.h"
#include "display_dc.h"

#include "defines.h"

ErrorStatus HSEStartUpStatus;
//--------------------------------------------------------------

void GPIO_Configuration(void){//настройка портов ввода-вывода
  
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB |
                         RCC_AHB1Periph_GPIOC  | RCC_AHB1Periph_GPIOD , ENABLE);
    
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//по умолчанию скорость 100МГц
 
  
//------------------------------------------------------------------------------------------------------------     
  //инициализируем входы
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  
  //сначала входы, которые влияют на формат изобряжения (тип экрана)  
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//притянуты кверху
  //порт B                       INVY
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //порт D                       INVX
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  //далее все ноги без притяжки
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
  //входы клавиатуры
  //порт B                         COL1           COL2          COL3
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //выходы
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//сначала с открытым коллектором  
  
    //порт C                        ROW1           ROW2          ROW3     
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//теперь выходы пушпульные
  GPIO_ResetBits(GPIOA, GPIO_Pin_6);//  GPIO_SetBits(GPIOA, GPIO_Pin_6); //D/#C в 1
    
  //порт А                            LED2        DIR2         D/#C         DIR1         LED1
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_11;            
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_ResetBits(GPIOB, GPIO_Pin_0);//GPIO_SetBits(GPIOB, GPIO_Pin_0); //ресет в 1
  //порт B                           #RES          LED_LNK       LED_RUN      LED_ALRM     LED_WRN        
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_1;//GPIO_Pin_13
  GPIO_Init(GPIOB, &GPIO_InitStructure);
    //порт А                         NSS             
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_4;            
  GPIO_Init(GPIOA, &GPIO_InitStructure);

//------------------------------------------------------------------------------------------------------------   
  //Инициализируем ноги, связанные с переферией:
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
  //подключаем альтернативные функции к нужным пинам:
  //GPIOA->AFR[0]  |= 0x00007700;//USART2
  //этот способ инициализации более громоздкий, зато более понятный:
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1); //Tx 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); //Rx
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //Tx 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); //Rx
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1); //SCK - OUT_SH(DIO_CLK)
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1); //MOSI - OUT_D 

  //настраиваем AF пины, которые push-pull, либо входы: 
  //порт А                         USART2_Tx    USART2_Rx    USART1_Tx    USART1_Rx
  GPIO_InitStructure.GPIO_Pin  =   GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
    //порт А                         NSS         SCK           MOSI     
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_5 | GPIO_Pin_7;            
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//******************************************************************************
//Таймер для работы с MODBUS клиентами
void TIM8_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);                        // разрешаем тактирование

  TIM_TimeBaseStructure.TIM_Prescaler = 168-1;                                // частота = TIM8CLK(168МГц)/168 = 1Мгц. один тик - микросекунда
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 0xffff;                                  // будет крутиться по кругу до переполнения 16 бит
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM8, ENABLE);
  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;       
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;  
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC1Init(TIM8, &TIM_OCInitStructure);
  
  TIM_ITConfig(TIM8, TIM_IT_Update, DISABLE);                                 // пока запретим прерывания
  TIM_Cmd(TIM8, ENABLE);
  TIM8->SR = 0;
}


//******************************************************************************
//Таймер общего назначения с прерыванием раз в 0,001 сек
void TIM6_Configuration(void){
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
   /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 10-1;//частота = TIM4CLK(84МГц)/10 = 8,4МГц.
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 8400;//частота переполнения 8,4Мгц/8400 = 1кГц
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM6, DISABLE);
  TIM6->SR = 0;
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM6, ENABLE);
}
//******************************************************************************
//настройка SPI для работы с платой дискретных входов/выходов
//работа в полном дуплексном режиме(отправка даных для DO, прием данных от DI)
//размер пакета 16 бит (по 16 входов и выходов)
//прескейлер выбран 256, скорость достаточна для опроса 100раз/секунду
void SPI1_Configuration(void){
  SPI_InitTypeDef  SPI_InitStructure;
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1, ENABLE);
  /* SPI1 configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
 // SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; //было 256
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  /* Enable SPI1 NSS output for master mode */
  SPI_SSOutputCmd(SPI1, ENABLE);
  /* Enable SPI1  */
  SPI_Cmd(SPI1, ENABLE);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/

#define APP_START_ADDR 0x08008000

//разрешаем переходы по векторам и настраиваем преоритеты следующих прерываний:
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  SCB->VTOR = APP_START_ADDR;//переносим начало вектора прерываний по указанному адресу
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* Enable TIM interrupts */
  
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;//таймер общего назначения(DIO)
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure);   
  
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//  Modbus master
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure); 

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//  Modbus slave
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure); 
    
}

void DMA_Configuration (void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); 
  SPI_Cmd(SPI1, DISABLE);
  DMA_InitTypeDef DMA_InitStructure;                                          // объявление структуры инициализации ДМА
  
  //настройка ДМА для АЦП
  DMA_InitStructure.DMA_Channel = DMA_Channel_3;                              // выбор канала ДМА
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;          // откуда берать данные
  DMA_InitStructure.DMA_Memory0BaseAddr = getFrameBufferAddr();//(uint32_t)&framebuffer[0][0];            // куда заталкивать данные
  DMA_InitStructure.DMA_BufferSize = 0;                                      // выбор размера буфера (я так понял в нашем случае количество каналов АЦП)
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            // запрещаем инкрементировать адрес периферии (откуда берём)
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     // разрешаем инкрементировать адрес памяти (куда пишем)
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // размер данных в периферии
  DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;         // размер данных в памяти
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                             // режим работы ДМА для непрерывных потоков данных (СканМод у АЦП)
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         // приоритет выбранного канала перед остальными
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                      // передача данных напрямую (без накапливания)
  DMA_InitStructure.DMA_FIFOThreshold       = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst          = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst       = DMA_PeripheralBurst_Single;
//  DMA_Init(DMA2_Stream0, &DMA_InitStructure);                                 // выбираем 0 поток, SPI1_Rx
 //Rx в принципе нам не нужен
  
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_Init(DMA2_Stream3, &DMA_InitStructure);                                 // выбираем 0 поток, SPI1_Tx

 // DMA_Cmd(DMA2_Stream0, ENABLE); 
  DMA_Cmd(DMA2_Stream3, ENABLE); 
  // разрешение работы DMA2_Stream0
  
  SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
  SPI_Cmd(SPI1, ENABLE);
}

//инициализация периферии
void Init (void){ 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); 
  GPIO_Configuration();//конфигурируем порты

  for(u32 InitDelay=12000; InitDelay>0; InitDelay--) {}
  
  SPI1_Configuration();
  ssd1305_init();
  //LCD_DATA;
  DMA_Configuration();
  TIM6_Configuration();
  NVIC_Configuration();//конфигурируем прерывания

}




