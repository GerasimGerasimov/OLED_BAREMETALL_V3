#include "init.h"
#include "stm32f4xx.h"

#include "ramdata.h"
#include "ModbusSlaveConf.h"
#include "STM32F4xx_Intmash_Flash.h"

ErrorStatus HSEStartUpStatus;

//--------------------------------------------------------------

void GPIO_Configuration(void){//��������� ������ �����-������
  
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB |
                          RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//
 
//------------------------------------------------------------------------------------------------------------     
  //�������������� �����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  
  //������� �����, ������� ������ �� ������ ����������� (��� ������)  
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//��������� ������
  //���� B                       INVY
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //���� D                       INVX
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  //����� ��� ���� ��� ��������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
  //����� ����������
  //���� B                         COL1           COL2          COL3
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //������
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//������� � �������� �����������  
  
    //���� C                        ROW1           ROW2          ROW3     
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//������ ������ ����������
  GPIO_ResetBits(GPIOA, GPIO_Pin_6);//  GPIO_SetBits(GPIOA, GPIO_Pin_6); //D/#C � 1
    
  //���� �                            LED2        DIR2         D/#C         DIR1         LED1
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_11;            
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_ResetBits(GPIOB, GPIO_Pin_0);//GPIO_SetBits(GPIOB, GPIO_Pin_0); //����� � 1
  //���� B                           #RES          LED_LNK       LED_RUN      LED_ALRM     LED_WRN        
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_1;//GPIO_Pin_13
  GPIO_Init(GPIOB, &GPIO_InitStructure);
    //���� �                         NSS             
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_4;            
  GPIO_Init(GPIOA, &GPIO_InitStructure);

//------------------------------------------------------------------------------------------------------------   
  //�������������� ����, ��������� � ����������:
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
  //���������� �������������� ������� � ������ �����:
  //GPIOA->AFR[0]  |= 0x00007700;//USART2
  //���� ������ ������������� ����� ����������, ���� ����� ��������:
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1); //Tx 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); //Rx
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //Tx 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); //Rx
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1); //SCK - OUT_SH(DIO_CLK)
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1); //MOSI - OUT_D 

  //����������� AF ����, ������� push-pull, ���� �����: 
  //���� �                         USART2_Tx    USART2_Rx    USART1_Tx    USART1_Rx
  GPIO_InitStructure.GPIO_Pin  =   GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
    //���� �                         NSS         SCK           MOSI     
  GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_5 | GPIO_Pin_7;            
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//******************************************************************************
//������ ��� ������ � MODBUS ���������
void TIM8_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);                        // ��������� ������������

  TIM_TimeBaseStructure.TIM_Prescaler = 168-1;                                // ������� = TIM8CLK(168���)/168 = 1���. ���� ��� - ������������
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 0xffff;                                  // ����� ��������� �� ����� �� ������������ 16 ���
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM8, ENABLE);
  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;       
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;  
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC1Init(TIM8, &TIM_OCInitStructure);
  
  TIM_ITConfig(TIM8, TIM_IT_Update, DISABLE);                                 // ���� �������� ����������
  TIM_Cmd(TIM8, ENABLE);
  TIM8->SR = 0;
}

//******************************************************************************
//������ ������ ���������� � ����������� ��� � 0,001 ���
void TIM6_Configuration(void){
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
   /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 10-1;//������� = TIM4CLK(84���)/10 = 8,4���.
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 8400;//������� ������������ 8,4���/8400 = 1���
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM6, DISABLE);
  TIM6->SR = 0;
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM6, ENABLE);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
//��������� �������� �� �������� � ����������� ���������� ��������� ����������:
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* Enable TIM interrupts */
  
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;//������ ������ ����������(DIO)
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

void Init (void){//��������� �������������. 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  GPIO_Configuration();//������������� ����
  ModbusClientInit();//�������������� ������ ����������� ������-�������  
  TIM6_Configuration();//������ ��� ���� ������� ��������� ����  
  NVIC_Configuration();//������������� ����������
}



