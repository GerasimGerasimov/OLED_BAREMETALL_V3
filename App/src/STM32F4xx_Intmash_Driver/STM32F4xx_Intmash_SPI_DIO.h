/**
  ******************************************************************************
  * @file    STM32F4xx_Intmash_SPI_DIO.h
  * @author  IMD, Erlina
  * @version V1.0.0
  * @date    16-06-2019
  * @brief   Файл содержит функции для работы дискретными входами выходами через SPI 
  ******************************************************************************
  * @Заметки:
  *
  * 
  ******************************************************************************
  * @Как с этим работать:
  * - Сначала необходимо созрать элемент UserDIOst типа Intmash_DIO и заполнить. вызвать функцию SpiDioInit();
  * - Затем вызывается DIO_Reset(&Intmash_DIO);  
  * - Для конфигурации SPI вызывается функция SPI_DIOdriverInit(&Intmash_DIO);   
  * - В таймере общего назначения (или любом другом с необходимой периодичностью 
  * вызова) вызывается функция DIOtransactionStart(&Intmash_DIO), обеспечивающая работы регистров. 
  * Частота вызова функции деленная на 2 - это частота опроса дискретных входов 
  * и изменения дискретных выходов. Функция возвращает 1, если состояние DI  
  * обновлено, можно подготовить новые данные DO и обрабатывать принятые. Функция
  * возвращает 0, если в текущий момент инициирован обмен данными между МК и DIO 
  * 
  * Вариант работы с таймером: функция DIOtransactionStart вызывается с нужной частотой где-то в коде.
  * В ней заряжается таймер на выставление защелки в нужное состояние
  ******************************************************************************
  */
#ifndef STM32F4xx_Intmash_SPI_DIO_H
#define STM32F4xx_Intmash_SPI_DIO_H
  
#include "stm32f4xx.h"
#include "Intmash_bastypes.h" 

//текущее состояние входов/выходов
extern tU16Union SPI_DIO_Inputs;
extern tU16Union SPI_DIO_Outputs;

/*
Структура конфигурации драйвера
*/
typedef struct 
{
  //используемый интерфейс SPI
  //НЕОБХОДИМО ИНИЦИАЛИЗИРОВАТЬ!!!
  SPI_TypeDef* SPIx;
  //указатель на таймер, используемый для защелки данных, если=0 - работает без таймера (Basic Timers: 6 или 7)
  TIM_TypeDef* LatchTimer;
  //порты, используемые для управления сдвиговыми регистрами (необходимо инициализировать!!!)
  GPIO_TypeDef* DO_OE_Port;
  GPIO_TypeDef* DO_RST_Port;
  GPIO_TypeDef* DO_LCLK_Port;
  GPIO_TypeDef* DI_CE_Port;
  GPIO_TypeDef* DI_LCLK_Port;
  //пины портов, используемые для управления сдвиговыми регистрами (необходимо инициализировать!!!)
  tU16 DO_OE_Pin;
  tU16 DO_RST_Pin;
  tU16 DO_LCLK_Pin;
  tU16 DI_CE_Pin;
  tU16 DI_LCLK_Pin;   
  //настройки SPI
  tU16 SPI_DataSize;//количество входов/выходов (по умолчанию SPI_DataSize_8b)
  tU16 SPI_BitOrder;//порядок подключения к сдвиговым регистрам (по умолчанию SPI_FirstBit_MSB)
  //настройки таймера (если=0 - работает без таймера)
  tU16 LatchTimerDelay;  
  //настройки режимов работы входов/выъодов
  tU8  InputsMode;
  tU8  OutputsMode;   
} SPI_DIO_InitStruct;  

#define DIO_MODE_NORMAL  0
#define DIO_MODE_INVERT  1

//Определения масок для дискретных входов/выходов
#define DIN0   Bits.B0  
#define DIN1   Bits.B1   
#define DIN2   Bits.B2
#define DIN3   Bits.B3
#define DIN4   Bits.B4
#define DIN5   Bits.B5
#define DIN6   Bits.B6
#define DIN7   Bits.B7
#define DIN8   Bits.B8
#define DIN9   Bits.B9
#define DIN10  Bits.B10
#define DIN11  Bits.B11
#define DIN12  Bits.B12
#define DIN13  Bits.B13
#define DIN14  Bits.B14
#define DIN15  Bits.B15

#define DOUT0   Bits.B0
#define DOUT1   Bits.B1
#define DOUT2   Bits.B2
#define DOUT3   Bits.B3
#define DOUT4   Bits.B4
#define DOUT5   Bits.B5
#define DOUT6   Bits.B6
#define DOUT7   Bits.B7
#define DOUT8   Bits.B8
#define DOUT9   Bits.B9
#define DOUT10  Bits.B10
#define DOUT11  Bits.B11
#define DOUT12  Bits.B12
#define DOUT13  Bits.B13
#define DOUT14  Bits.B14
#define DOUT15  Bits.B15

//декларирование функций
void SPI_DIO_DriverInit(SPI_DIO_InitStruct * DIOstr);//Инициализация драйвера
void SPI_DIO_Reset(void);//сброс сигналов управления и регистров в исходное состояние
tU8  SPI_DIO_Processing(void);//прием DI, отправка DO, защелка в регистры. 
void SPI_DIO_ProcessingTimer(void);//прием DI, защелка в регистры.

#endif 