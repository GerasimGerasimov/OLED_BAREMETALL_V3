/**
  ******************************************************************************
  * @file    STM32F4xx_Intmash_SPI_FRAMkey.с
  * @author  IMD, Erlina
  * @version V1.0.0
  * @date    16-06-2019
  * @brief   Файл содержит функции для работы с с ключом параметров на Fram  FM25040B-G
  * 
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
  * 
  ******************************************************************************
  */
#include "STM32F4xx_Intmash_SPI_FRAMkey.h"
#include "crc16.h"//модуль с функциями для расчета контрольной суммы массива

// команды для работы с FRAM
#define FM_READ   0x03      //Read Status Register 
#define FM_WRITE  0x02     //Write Status Register
#define FM_WREN   0x06       //Set Write Enable Latch
#define FM_WRDI   0x04       //Write Disable 	
#define FM_RDSR   0x05       //Read Status Register
#define FM_WRSR   0x01       //Write Status Register

#define CS_ST  (CS_Port->ODR & CS_Pin)
#define CS_UP  CS_Port->BSRRL=CS_Pin
#define CS_DWN CS_Port->BSRRH=CS_Pin

#define DELAY for(tU16 i=30;i;i--){}  
#define WAIT_TRANSMIT while((FRAM_SPI->SR & SPI_I2S_FLAG_TXE) == RESET)

//переменные модуля
tU8 FRAMbuf[FRAM_KEY_SECTOR_SIZE];
SPI_TypeDef* FRAM_SPI;
GPIO_TypeDef* CS_Port;
tU16 CS_Pin; 

tU8Union SPI_FRAMkey_Status;

/*Инициализация драйвера. 
Перед вызовом этой функции необходимо включить тактирование GPIO*/
void SPI_FRAMkey_Init(SPI_FRAMkey_InitStruct * Key)
{
  FRAM_SPI = Key->SPIx;
  CS_Port = Key->GPIOxCS;
  CS_Pin = Key->Pin_CS;
    
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;        
  GPIO_InitStructure.GPIO_Pin  =  CS_Pin;
  GPIO_Init(CS_Port, &GPIO_InitStructure);
  
  SPI_InitTypeDef  SPI_InitStructure;
  if(FRAM_SPI == SPI1) RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1, ENABLE);
  else if(FRAM_SPI == SPI2) RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE);
  else if(FRAM_SPI == SPI3) RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI3, ENABLE);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(FRAM_SPI, &SPI_InitStructure);

  /* Enable SPIx  */
  SPI_Cmd(FRAM_SPI, ENABLE);
}


/**
  * @brief  записать весь массив даных
  * @param  *Src - указатель на массив, откуда берем данные для записи
            ByteNum - количество записываемых байт (от 1 до 256)
            Addr - адрес, с которого начинать брать данные/куда записывать (от 0 до 255)
            FRAMwriteSector - основной сектор, FRAMwriteBKP - backup 
  * @retval none
  */
void SPI_FRAMkey_Write(tU8 * Src, tU8 Sector, tU8 Addr, tU16 ByteNum)
{
  CS_UP;
  DELAY;
  CS_DWN;
  DELAY;
  
  FRAM_SPI->DR = FM_WRSR; 
  WAIT_TRANSMIT;
  FRAM_SPI->DR = 0x00; 
  WAIT_TRANSMIT;
  
  CS_UP;
  DELAY;
  CS_DWN;
  DELAY;
    
  FRAM_SPI->DR = FM_WREN;
  WAIT_TRANSMIT;
  
  CS_UP;
  DELAY;
  CS_DWN;
  DELAY;
  
  FRAM_SPI->DR = FM_WRITE + Sector*8;
  WAIT_TRANSMIT;
  FRAM_SPI->DR = Addr;
  WAIT_TRANSMIT;
  
  //а теперь собственно пишем массив, n число байт
  for(tU16 i= 0 ; i<ByteNum ;i++)
  {
    FRAM_SPI->DR = *Src++;
    WAIT_TRANSMIT;
  }
  
  CS_UP;
}

/**
  * @brief  чтение массива данных
  * @param  *Dst - указатель на массив, куда записываем считанные данные
            ByteNum - количество считываемых байт (от 1 до 256)
            Addr - адрес, с которого начинать брать данные/куда записывать (от 0 до 255)
            FRAMreadSector - основной сектор, FRAMreadBKP - backup 
  * @retval none
  */                                                                    
void SPI_FRAMkey_Read(tU8 * Dst, tU8 Sector, tU8 Addr, tU16 ByteNum)
{
  CS_UP;
  DELAY;
  CS_DWN;
  DELAY;
  
  FRAM_SPI->DR = FM_READ + Sector*8;
  WAIT_TRANSMIT;
  FRAM_SPI->DR = Addr;
  WAIT_TRANSMIT;

  DELAY;
  DELAY;

  //читаем массив данных
  for(tU16 i=0; i<ByteNum; i++)
  {
    FRAM_SPI->DR = 0xff;
    while (((FRAM_SPI->SR & (SPI_I2S_FLAG_TXE | SPI_I2S_FLAG_RXNE)) == 0 || (FRAM_SPI->SR & SPI_I2S_FLAG_BSY)));
    *Dst++ = FRAM_SPI->DR;
  }
  DELAY;
  CS_UP;
}  


/***
  * @brief  прочитать данные статус-регистра fram (есть запрет записи или нет)
  * @param  none 
  * @retval прочитанный байт данных
 
tU8 ReadSR(Intmash_FramKey * UserFramKeyStr)   
{   
  tU8 cData = 0;   
  CS_DWN(UserFramKeyStr->GPIOxCS, UserFramKeyStr->Pin_CS);
  for(tU16 i=delay;i;i--){}
  UserFramKeyStr->SPIx->DR = RDSR;
  while(SPI_I2S_GetFlagStatus(UserFramKeyStr->SPIx,SPI_I2S_FLAG_TXE) == RESET);
  UserFramKeyStr->SPIx->DR = 0xff;
  while (((UserFramKeyStr->SPIx->SR & (SPI_I2S_FLAG_TXE | SPI_I2S_FLAG_RXNE)) == 0 || (UserFramKeyStr->SPIx->SR & SPI_I2S_FLAG_BSY)));
  cData=UserFramKeyStr->SPIx->DR;  
  
  CS_UP(UserFramKeyStr->GPIOxCS, UserFramKeyStr->Pin_CS);   

  return cData;   
} */

/**
  * @brief  проверка фрам
  * @param none
  * @retval CRC_16
  */
void SPI_FRAMkey_Check(void)
{
  //читаем основной сектор фрам
  SPI_FRAMkey_Read(FRAMbuf, FRAM_KEY_MAIN_SECTOR, FRAM_KEY_DEFAULT_ADDR, FRAM_KEY_SECTOR_SIZE);
  if (crc16((tU8*)FRAMbuf, FRAM_KEY_SECTOR_SIZE))
  //контрольная сумма не сошлась, сектор битый или после прошивки
  {        
    //читаем резервный сектор фрам
    SPI_FRAMkey_Read(FRAMbuf, FRAM_KEY_BKP_SECTOR, FRAM_KEY_DEFAULT_ADDR, FRAM_KEY_SECTOR_SIZE);
    if (crc16((tU8*)FRAMbuf, FRAM_KEY_SECTOR_SIZE))
    {
      //оба сектора битые или после прошивки
      SPI_FRAMkey_Status.Bits.MAIN_SECTOR_ERROR=1;
      SPI_FRAMkey_Status.Bits.BKP_SECTOR_ERROR=1;
    }
    else  
    {
      SPI_FRAMkey_Status.Bits.BKP_SECTOR_ERROR=0;
      //пытаемся восстановить из бэкапа
      SPI_FRAMkey_Write(FRAMbuf, FRAM_KEY_MAIN_SECTOR, FRAM_KEY_DEFAULT_ADDR, FRAM_KEY_SECTOR_SIZE);
      if (crc16((tU8*)FRAMbuf, FRAM_KEY_SECTOR_SIZE))    SPI_FRAMkey_Status.Bits.MAIN_SECTOR_ERROR=1; //не получилось, ставим ошибку
      else  SPI_FRAMkey_Status.Bits.MAIN_SECTOR_ERROR=0;
    }    
  }
}
