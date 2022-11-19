/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "ramdata.h"
#include "flashdata.h"
#include "CalibrationData.h"
#include "init.h"
#include "id.h"
#include "defines.h"
#include "display_driver.h"
#include "app.h"
//#include "ssd1305.h"
//#include "Buttons_action.h"   
//#include "display.h"   
//#include "Screens.h"

#include "ModbusMasterConf.h"
#include "ModbusSlaveConf.h"

//#include "com_data_valid.h"

//#include "resources.h"

/* defines ------------------------------------------------------------------*/
#define OVERTURN_X_IN  (GPIOD->IDR & GPIO_Pin_2) 
#define OVERTURN_Y_IN  (GPIOB->IDR & GPIO_Pin_3) 
/* Private variables ---------------------------------------------------------*/ 
u16 btn_scan_code  = 0; //скан код кнопки. сбрасывается в 0 после вызова обработки дейтсвия на кнопку
u8 Actual_Scr = 0; //переменная, отображающая - какой сейчас экран на дисплее
u8 ScrDraw_update=0;
u8 PowerLogScrNum=0;
u8 BlackBoxScrNum=0;
u8 NumLogStr=0;

tU16 LocalPassword=0;

static volatile tU8 OverturnX = 0;

void update(tU8 Overturn);
    

void popUpAlarmScreen(void);

int main(void) {  
  BootLoadCmdFillZero();  
  Init();
  //TGrahics::init();
  App::init();
  //App::run();
  IDinit();
  
  if(FLASH_DATA.Overturn.Coord.X) OverturnX = FLASH_DATA.Overturn.Coord.X - 1;
  else OverturnX = (OVERTURN_X_IN) ? 0 : 1;
  
  ModbusClientInit(); //слейв для проверки
  ModbusMasterInit(); //мастер  
  
  LED_RUN_ON;
  LED_WRN_OFF;
  LED_LINK_OFF;
  LED_ALARM_OFF;
  
  LED2_OFF;
  LED1_ON;
  
  __enable_irq(); 
  
  PowerLogScrNum = 0xff; //присвоили номера экранов лога
  BlackBoxScrNum = 0xff; //но у этой прошивке у нас нет ни лога ни черного ящика, так что им заведомо неиспользуемые номера
  
  //clean_screen();
  //инициализируем стурктуру первой страницей = меню
  //Common_scr_init(&Pages[1]);
  //DrawActualScreen(Luc_Cons_7pt);  
  //draw_string(1, 32, MAX_END_X, str_one, Luc_Cons_7pt, "Ожидание ...");
  update(OverturnX);  
  
  //initAlarmsPage();

  slot_FLASH_READ.Flags.Bits.SKIP_SLOT = 0; //считали для начала и все
  slot_CD_READ.Flags.Bits.SKIP_SLOT = 0;

//------------------------------------------------------------------------------
  while (1)
  { 
   //если надо обновить экран - каждый раз при входе на новый+ при каждом чтении рамки, если соммон скрин + при нажатии любой кнопки
    if (ScrDraw_update) //пока всегда обновлять по флагу
    {
      //clean_screen();
      popUpAlarmScreen();
      //DrawActualScreen(Luc_Cons_7pt);   

      update(OverturnX);

      ScrDraw_update = 0; //сбросили флаг обновления
    }   
    //if (btn_scan_code) ButtonsAction(); //обработали нажатость кнопок, если она была
       
    //--работа интерфейсов связи----------------------------------------------------
    //check_leds();
    //ModbusMasterSlotSelection(&CommonMaster); 
    
    if (((slot_RTR.Flags.Bits.TIMEOUT_ERR)||(slot_RTR.Flags.Bits.CRC_ERR))||((FLASH_DATA.Retranslate == 0)&&(fRtrWaitResp != 0)))  //если была проблема со связью - сбросить флаги и настроить на прием
    {
      fRtrWaitResp =0;
      slot_RTR.Flags.Bits.SKIP_SLOT = 1;
      slot_RTR.Flags.Bits.TIMEOUT_ERR =0;
      slot_RTR.Flags.Bits.CRC_ERR =0;
      UsartRecieve(&UART1toRS485, &RS485slave.Buffer[0]);    
    }

  }  
}

void popUpAlarmScreen(void) {
  //static bool allreadyset = false;

  /*
  if (isRAMDataValid()) {
    sortPageByAlarms();
    if (Common_scr_struct.num_cur == 4) {
      Common_scr_struct.page_size = Pages[4].page_size;
    }
    if (isAlarms()) {
      if (!allreadyset) {
        Common_scr_init(&Pages[4]);
        allreadyset = true;
      }
     } else {
       allreadyset = false;
     }
  }
*/
}
/*функция обновления экрана*/
void update(tU8 Overturn)
{  
  
  //if(Overturn) overturn_screen(Overturn, 0 /*OverturnY*/);
  
  /*
  ssd1305_rst_first();
  DMA_Cmd(DMA2_Stream3, DISABLE);  //TX
  DMA2->LIFCR = (uint32_t) (DMA_FLAG_FEIF3 | DMA_FLAG_DMEIF3 | DMA_FLAG_TEIF3 | DMA_FLAG_HTIF3 | DMA_FLAG_TCIF3);
  DMA2_Stream3->NDTR = 1024;
  DMA_Cmd(DMA2_Stream3, ENABLE); //TX
    
  if ( DMA2_Stream3->NDTR!=0)
   {
     while (DMA2_Stream3->NDTR);
   }
  ssd1305_rst_second();
  */
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
