#include "stm32f4xx_it.h"

#include "defines.h"
#include "ramdata.h"
#include "flashdata.h"
#include "timer_wrapper.h"
#include "leds_timer_wrapper.h"

u16 num = 0;

#include "ModbusMasterConf.h"
#include "ModbusSlaveConf.h"

void TIM6_DAC_IRQHandler(void)//таймер общего назначения 1 мс
{ 
  
    TIM6->SR = 0;//&= ~TIM_SR_UIF;// снять флаг прерывания
    LedsTimerTick();
    sendTimerMessage();
    /*
    if (num == 500)
    {
       if (F3_but_ok == F3_BUTTON_ON)
       {
         if (LED_RUN_ST)  LED_RUN_ON;
        else LED_RUN_OFF;
       }
       num = 0;

    }
    else num++;
    buttons_scan(buttons_check()); //проверка нажатой кнопки и подавление дребезга
    
 
      if (RetrLinkCnt == 1500)
      {
        if (LED1_ST) LED1_OFF; else LED1_ON; //моргнули светиком 

        RetrLinkCnt =0;  //попробуем скипнуть все слоты
        slot_RTR.Flags.Bits.TIMEOUT_ERR=1;
        slot_RTR.Flags.Bits.SKIP_SLOT=1;
        slot_FLASH_READ.Flags.Bits.SKIP_SLOT=1;
        slot_WRITE.Flags.Bits.SKIP_SLOT=1;
        slot_RAM_READ.Flags.Bits.SKIP_SLOT=1;
        //и сбросить все прерывания модбаса?
        TIM7->SR = 0;
        StopMasterTimer();  
        USART1->SR =0;
        USART2->SR =0;
        CommonMaster.Status = SELECTION;
//        RebootMBMaster();
//        RebootMBSlave();
      }
      else RetrLinkCnt++;  //моргнули светиком 
      */
}

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  //TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
