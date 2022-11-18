/**
  ******************************************************************************
  * @file    STM32F4xx_Intmash_SPI_FRAMkey.h
  * @author  IMD, Erlina
  * @version V1.0.0
  * @date    16-06-2019
  * @brief   Файл содержит определения, макросы и декларирование функций для 
  * работы с ключом параметров на Fram FM25040B-G
  ******************************************************************************
  * @Как с этим работать:
  * - Сначала пины портов микроконтроллера, которые являются управляющими 
  * сигналами FRAM. Далее в init.c конфигурируются порты микроконтроллера. 
  * - В файле fram.h редактируются макросы для сигналов управления для данной 
  * аппаратной реализации (если необходимо).
  * - В функии инициализации устройства вызывается функция CheckFRAMkey(), которая
  * проверяет работу ключа параметров и целостность данных, на нем записаных.
  * - Если вызвана функция записи во собственную flash память устройства, 
  * вызвать функцию FRAMwriteMassive для записи в основной и резервный сектора 
  * ключа параметров измененные данные из flash памяти (чтобы на ключе 
  * параметров всегда были актуальные данные)
  * - Так же нижеописанные функции можно вызывать по необходимости (например, 
  * если flash-память повреждена - записать в неё уставки из ключа параметром, 
  * предварительно считав её с помощью FRAMreadMassive)
  ******************************************************************************
  * @Заметки:
  *
  * 
  ******************************************************************************
  */

#ifndef STM32F4xx_Intmash_SPI_FRAMkey_H
#define STM32F4xx_Intmash_SPI_FRAMkey_H

#include "stm32f4xx.h"//библиотека StdPeriph_Driver
#include "Intmash_bastypes.h" 

#define FRAM_KEY_DEFAULT_ADDR 0
#define FRAM_KEY_MAIN_SECTOR  0 //основной сектор
#define FRAM_KEY_BKP_SECTOR   1 //резервный
#define FRAM_KEY_SECTOR_SIZE  256

//статусные флаги 
#define MAIN_SECTOR_ERROR  B0     
#define BKP_SECTOR_ERROR   B1    

typedef struct 
{
  SPI_TypeDef* SPIx;
  GPIO_TypeDef* GPIOxCS;
  tU16 Pin_CS; 
} SPI_FRAMkey_InitStruct;  

extern tU8Union SPI_FRAMkey_Status;

void SPI_FRAMkey_Init(SPI_FRAMkey_InitStruct * Key);
void SPI_FRAMkey_Write(tU8 * Src, tU8 Sector, tU8 Addr, tU16 ByteNum);
void SPI_FRAMkey_Read(tU8 * Dst, tU8 Sector, tU8 Addr, tU16 ByteNum);
void SPI_FRAMkey_Check(void);



#endif 