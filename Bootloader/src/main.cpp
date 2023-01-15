#include "stm32f4xx.h"
#include "ModbusSlaveConf.h"

#include "init.h"
#include "id.h"

#include "DExS.M4CPU_BoardLEDs.h"
//////////////////////////
#include "ramdata.h"

#include "bootloader.h"
                         
const char * IDtext = "Bootloader v1.1.1 23.12.2022 www.intmash.ru";

int main(void) {
  
  GPIO_INIT_Configuration();
  INIT_ON;
  if (INIT_IST != 0) {
    LED5_OFF;
    if (isBootLoaderMustBeStart() != true) {
      
      if ( isApplicationReadyToStart()) {
        jumpToApplication();
      }
    }
  }
  
  IDinit(1, (char *) IDtext);
  Init();
  LED5_ON;
  LED_WRN_OFF; 
  LED_RUN_OFF;
  LED_LINK_OFF;
  LED_ALARM_OFF;
  
  __enable_irq(); 

  while (1)
  { 
    (INIT_IST)? LED5_OFF : LED5_ON;
    ////////////////////////////////////////////////////////////////////////////    
    if (RAM_DATA.FLAGS0.bits.T1000Hz) {
      RAM_DATA.FLAGS0.bits.T1000Hz = 0;
      static tU16 blink = 1000;
      (blink)
        ? (blink--)
        : ((LED_ALARM_ST) ? (LED_ALARM_ON) : (LED_ALARM_OFF), blink = 1000);
    }
  }  
}

#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
