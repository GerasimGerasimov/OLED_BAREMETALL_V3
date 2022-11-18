#include "STM32F4xx_intmash_MBbasicCommands.h"
#include "STM32F4xx_Intmash_USART.h"
#include "ModbusSlave.h"
#include "Intmash_bastypes.h" 
#include "DExS.M4CPU_BoardLEDs.h"
#include "STM32F4xx_Intmash_Flash.h"
#include "RAMData.h"
#include "crc16.h"
#include "bootloader.h"

//��������� ��������� UART � �������
Intmash_Usart UART1toRS485; // ����� �� ������������� RS485
ModbusSlaveType RS485slave;

//����� ������������ ������ +1
ModbusCommandHandlerType ModbusCommands[3]={
  {GetDeviceID, 0x11},
  {BootLoader, 0xB0},
  {0, 0},
}; 


#define INTERFACE_DEFAULT_SETTINGS 0x00000401

void ModbusClientInit(void) //������ ������������� ���������, ������
{ 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//
  
  //tU32Union InterfaceSettings;
  
  //USART1, OptRS485
  UART1toRS485.USARTx = USART1;
  //if(FlashStatus.Bits.FLASH_DATA_ERR) InterfaceSettings.I=INTERFACE_DEFAULT_SETTINGS; 
  //else InterfaceSettings.I=FLASH_DATA.Modbus_U1_RS485;

  UART1toRS485.USART_BaudRate = 115200;//USARTbaudRate[InterfaceSettings.B[1]];
  RS485slave.SlaveAddress = 1;//InterfaceSettings.B[0];
  UART1toRS485.USART_StopBits = 0;// 0;//����-��� �� ��������� 1 InterfaceSettings.B[2];
  UART1toRS485.USART_Parity = 0 ;//��� �������� �� ��������� ����������� InterfaceSettings.B[3];

  UART1toRS485.DMAy_StreamRX = DMA2_Stream5;
  UART1toRS485.DMAy_StreamTX = DMA2_Stream7;
  
  UART1toRS485.DMA_channel = DMA_Channel_4;
  UART1toRS485.DMA_FLAGS_RX = DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5 | DMA_FLAG_TCIF5;
  UART1toRS485.DMA_FLAGS_TX = DMA_FLAG_FEIF7 | DMA_FLAG_DMEIF7 | DMA_FLAG_TEIF7 | DMA_FLAG_HTIF7 | DMA_FLAG_TCIF7;
  UART1toRS485.GPIOxDIR = GPIOA;
  UART1toRS485.GPIO_PinDIR = GPIO_Pin_8;
  UART1toRS485.PolarityDIR = POLARITY_HI; //��������� ���������� 
  UsartDriverInit (&UART1toRS485); //������������� ���������
  UsartRecieve (&UART1toRS485, &RS485slave.Buffer[0]);//��������� �� ����� � ����
}

//���������� USART �� ������������� RS485
void USART1_IRQHandler(void)
{
  tU16 MsgSize = UsartTxRxFinish(&UART1toRS485);//���������� ��� ���������: ����� ��� ������� MsgSize ����.
  if(MsgSize){//���� ��������  
    if(LED_LINK_ST) LED_LINK_ON; else LED_LINK_OFF; //�������� ��������
    //�������� ������� �������������, ���������� ������ ������ ��� ��������
    MsgSize = ModbusCommandDecode(&RS485slave, MsgSize, ModbusCommands); 
    
    //���� ���� ��� ���������� - ����������, ���� ��� - �������� �� �����.
    if(MsgSize) UsartTransmit(&UART1toRS485, &RS485slave.Buffer[0], MsgSize);
    else UsartRecieve(&UART1toRS485, &RS485slave.Buffer[0]);
  }  
  else {
    UsartRecieve(&UART1toRS485, &RS485slave.Buffer[0]);//���� ����������, �������� �� �����.
  }
}














