#ifndef MODBUSMASTER_H
#define MODBUSMASTER_H
/*****************************************************************************************************
������ ������ ������������� �������� ��� ������������� Modbus-�������.

Modbus-������ �������� �� ���������� ���������:
1)�������� ������ �������� ������������ ����� (������ ������� SELECTION). ���� ���������� ����� ���, 
� ���� �� ��������� ���� �������� ����� (SKIP), ���� ����� ������ �� ������� - ����� SKIP ������������ 
� ������ � ������������ ������ ��������� �������� (REPEATING_TRANSMIT);

2)����������� ���� ������, �������� �������� ��� ������� ������ (������ ������� TRANSMIT);

3)�������� ������� ������ ���������, ���� ����� ��������� ���������, ���� ��� ���������� (RECIEVE);

4)����� ������ ��������� ���������, ���� ���� ��������� ��������� ����� �� �����, ���� ���� ��������
������� - ���� ��������� ����������, ������������ ��� ���� SKIP, ��� ��� �������� ��� ��������� ����
������ ������������ �����, ��� ���� ������ ��������� � ��������� ����� ����� ����������� ���������� 
����� (������ PAUSE).

5) �� ���������� � ������ 1.   



��� ������ ������� ����������:
1) ������� ��������� ���� ������� (MBmasterType).

2) ������� ��������� ���� ������ ������� (MBmasterSlotType). ��� ������ �������� ����� ������� 
�� ���������� �����, �� ����� ����������� ���� ��� ���������, ������� �� � ������� ������. 

3) ���������������� ��������� �������, ���������� ������:
- ��������� �� �������, ���������������� ��������� �������� (�������� ������� - �������� � ��);
- ��������� �� �������, ��������������� ��������� �������� (������ ��������, ��������� ��������);
- ����� �������� � �� (�������� ������ ��� ���� ������ ����������);
- ����� ����� ����� ����������� ���� ������ � �� (��� ���� ����������);
- ���������������� ����� ��������� ������ � ������� ����������� ����;
- ��������� ����� ���������� �� �������������� �����, ������� � 0 �������, ���������������, ��� 
��������� ��������, ������� ������� ����� ���������� ����� ���������������� ����� (�����������).
- ��� ������������� �������� ����� ������ ����� (����� ��������� ���������);

4) ���������������� ����� ��� ��������� �������� (������ ����):

 - ��������� ����� �������� ����� ���������� ���������, ��� ����� ���������������� ����� �������� 
����� (���� ��������� �� ����� �������� � ������� ������ �����, ��������, ����� ���� ��������� 
���������� �������� ������ ������������� ������ ��������� � ���������� � ������������� �������);

��� ������������� ����� ���������� ������ ��������� ������� ������������: 
- ���������, ������� ��������� �������� ��� �������� ���������, �������� �������� ��������� (�����
�����) ����� �����-���� ��������� (��� �� ����� ����� ��������� ������� ��������� ��������, ���� 
���� �������������).
- ���������, ������� ��������� �������� ����� �������� ���������, �������� ������������� ������ 
������ �����-���� �����������.
- ���������, ������� ��������� �������� ����� ������ ��������� (���������), �������� ���������
�������� ������ (��������������).

- ����� ����� ���� ���������������� ������, ���� ������������ ���� SKIP (���� ��� �������������
� ���������� ����� ����� ����� ������ ������ �������, � ������ �� ������������� �������������),
��� ���� REPEATING_TRANSMIT(���� ����� ������� ���������� ����� ���������� ��������� ����������, 
�������� ��� ����������� ������ ������ � ������� ����������).

5) ��� ��������� �� �������, ���������� ���� ������ ������������ � ���� ����������������.

6) ���������� � ���� ������� ���������� ��������. 
void ModbusMasterSlotSelection(MBmasterType* Master); - ������� ����������� ����� ����� ��� ����������
(�����������, ���� ������ ������� SELECTION) � �������� ��� ������� ������, ���� �������� ���� ��� 
������. ����� ������������ � ����������� �� �������. ������ ���������� � main. 
  
void ModbusMasterTransferProcessing(MBmasterType* Master, tU16 TransferStatus); - �������, ����������
����� �������� ������� (TransferStatus = 0) ��� ������ ������ �� ������(TransferStatus = ������ 
��������� ��������� � ������). ����� �������� ����������� ������ �� ����� (���� �� ������� ���������
����� �������� - ���� ��������� ����������), ����� ������ ��������� ����� �� ����������, ���������� 
������ ��� �������� ��������� ��������� ������. ������ ���������� � ���������� ���������� (��, Rx).
 
- void ModbusMasterTimerProcessing (MBmasterType* Master); - ������� ���������� � ���������� �������, 
��������������� ��������� ��������. � ����������� �� �������� ������� ������� (PAUSE ��� RECIEVE) 
���� ���������� ������ SELECTION, ���� ���������� ���� �������� � ������� �����.

- void ActivateSlot(MBmasterSlotType* Slot); - ������� ���������� ���� SKIP � ���������� ����� (�����
�� ��� ���� ������ ��� ������ �������� ������ ��� ����������), ���������� ��� ������, �� �������������.

��� ������� ������ ������� ���������� ��� ������ ���������������� ��������� SELECTION.

������ ������������ ������ � ����������� � ����� ������� �����.
******************************************************************************************************/

#include "Intmash_bastypes.h"


#define MB_MASTER_BUF_SIZE  256
#define MB_MASTER_SLOTS_MAX 1

//���� ��� ����������� ������� ����������
typedef struct {  
  //����� ��������� ��� ��������   
  tU8 OutBuf [MB_MASTER_BUF_SIZE]; 
  tU8 InBuf [MB_MASTER_BUF_SIZE]; 
  //��������� ������� ���������� ���� ������ �� ��������
  void (*OnTimeOut)(void);
  //��� ������ ����� ��������� �����-�� ������
  void (*OnRecieve)(void);
  //����� (������, ��������� �����)
  tU8Union Flags;
  u8 InBufLen;
} MBmasterSlotType;


typedef struct {  
  //����� ���������   
  tU8 InBuf[MB_MASTER_BUF_SIZE]; 
  //����� ���������� �� �����, �������������� ������ ��������
  MBmasterSlotType* Slots[MB_MASTER_SLOTS_MAX];
  //��������� ������� �������� ������ �� ������ �������� (� ms)
  void (*SetTimer)(tU16 Time);
  //��������� ��������� �������
  void (*StopTimer)(void);
  //��������� ��������� ������� (� ms)
  tU16 TimeOut;//�������
  tU16 Pause;//����� ����� ����� ������������
  //������� ����������� ����
  tU16 CurrentSlot;
  //��������� ��������� ������
  tU16 LastError;
  //������ ������ ������� (��� ������ ������)   
  tU16  Status;
} MBmasterType;
//������� ������ �������
#define SELECTION 1
#define TRANSMIT  2
#define RECIEVE   4
#define PAUSE     8
//����� ������ � LastError
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

/***������ ������������*********************************************************************************
//��������� ������� � ������
MBmasterType CommonMaster;
MBmasterSlotType PwrRead;



//������ �������: ��������� �������, ��������� �������, ����������.
void SetMasterTimer(tU16 delay)
{  
  TIM7->CNT = 0;
  TIM7->ARR = delay*10;//�������� �� ������ �����.
  TIM7->SR = 0; //����� ���� ���������� 
  TIM_Cmd(TIM7, ENABLE);//��������� �����, ���� ����� ����� ���������.
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


//��������� �������: ��������� ��������, ����������
Intmash_Usart UARTtoCRB;
void USART6_IRQHandler(void)
{  
  ModbusMasterTransferProcessing(&CommonMaster, UsartTxRxFinish(&UARTtoCRB));
}

//������� �������� ����� PwrRead
void PwrReadLoad (void)//�������� ����� �� �������� 
{
  UsartTransmit(&UARTtoCRB, PwrRead.OutBuf, 8); //����� ��� ����������� � ������, ������ ���������� ��� ������ ��� � ������ ���� 
}
void PwrReadWaitAnswer (void)//��������� ���� � �������� ������
{
  UsartRecieve(&UARTtoCRB, CommonMaster.InBuf); //����������� ���� �� ���� ��������� 
}
void PwrReadFinish (void)//��������� ������ 
{  
  tU16 SwapTmp;  
  tU8  SwapIdx=MB_DATA_SECTION_CMD_03;  
  
  ModbusSwapCopy((tU8*)&CommonMaster.InBuf[SwapIdx], &SwapTmp, 1);
  fRi = ((float)SwapTmp);  
  SwapIdx+=2;
  
  ModbusSwapCopy((tU8*)&CommonMaster.InBuf[SwapIdx], &SwapTmp, 1);
  fXi = ((float)SwapTmp);    
}


//������������� ������� � �����������
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
  //������ ������� �� ������������� ������
  FrameEndCrc16(PwrRead.OutBuf, READ_CMD_SIZE);
  
  //������������� ������� UASRT
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE); 
  UARTtoCRB.USARTx = USART6;
  UARTtoCRB.USART_BaudRate = 115200;
  UARTtoCRB.DMAy_StreamRX = DMA2_Stream1;
  UARTtoCRB.DMAy_StreamTX = DMA2_Stream6;
  UARTtoCRB.DMA_channel = DMA_Channel_5;
  UARTtoCRB.DMA_FLAGS_RX = DMA_FLAG_FEIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1 | DMA_FLAG_TCIF1;
  UARTtoCRB.DMA_FLAGS_TX = DMA_FLAG_FEIF6 | DMA_FLAG_DMEIF6 | DMA_FLAG_TEIF6 | DMA_FLAG_HTIF6 | DMA_FLAG_TCIF6;  
  UARTtoCRB.PolarityDIR = WITHOUT_DIR; //���� ���
  UsartDriverInit (&UARTtoCRB); //������������� ���������  
      
  //������ ��� �������
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
  TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;//������� = TIM4CLK(84���)/8400 = 10���.
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 0xffff;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM7, DISABLE);    
  TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM7, DISABLE);//������ ���������� ����� �������� ������� ���� ����������
  TIM7->SR = 0;  
    
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;   
  NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;//������ ������ ����������
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&NVIC_InitStructure); 
}
******************************************************************************************************/


#endif
