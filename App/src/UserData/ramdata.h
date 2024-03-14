#ifndef RAMDATA_H
#define RAMDATA_H
#include "Intmash_bastypes.h"//содержит описания базовых типов
///////////////////////////////////////////////////////////////////////////////
//параметры расположеные в RAM
typedef  struct {  
    tU16Union FLAGS;    //00
    tU16 Last_lnk_error;//01
    
    tU16 var1; //02
    tU16 var2; //03
    tU16 UsedMem; //04
    tU16 TotalMem; //05
    tU16 KeyCode;
    tU16 DIO;
        
} TRAM_DATA;

//определение диапазона доступных регистров
#define RAM_DATA_PREFIX 0x0
#define RAM_DATA_SIZE 0x50

//объявление 
extern TRAM_DATA RAM_DATA;

///////////////////////////////////////////////////////////////////////////////
//определение флагов неисправностей
#define FLASH_ERR          B0
#define FLASH_BKP_ERR      B1
#define FRAM_KEY_ERR       B2
#define FRAM_KEY_BKP_ERR   B3



#endif