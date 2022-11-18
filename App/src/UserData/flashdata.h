#ifndef FLASHDATA_H
#define FLASHDATA_H
#include "Intmash_bastypes.h"//содержит описания базовых типов
//параметры расположеные FLASH
typedef union _tOverturnXY {
	tU16 I;
        struct {
	tU8 X;//младший
        tU8 Y;//старший
        } Coord;

} tOverturnXY;

typedef struct {
    
    tU32 Modbus_U1_RS485;//0   адрес и скорость для работы с мастером  
    tU32 Modbus_U2_RS485;//1 адрес и скорость для работы с слейвом   
    tU16 TIME_OUT_Lnkmngr; //уставки для связи линкменеджера - время тайм аута, 10=1мс /5000
    tU16 TIME_CHANGE_Lnkmngr; //время смены слотов 10=1мс /500
    float Koef_null;
    float Koef[20];
    tU16 Password;
    tU16 Retranslate;
    tOverturnXY Overturn;
    tU16 __free__;
 } TFLASH_DATA;

//определение диапазона доступных регистров
#define FLASH_DATA_PREFIX 0x2
#define FLASH_DATA_SIZE 0xfe

//параметры расположеные FLASH
extern const TFLASH_DATA FLASH_DATA;
extern const TFLASH_DATA BKFLASH_DATA;

#endif 