#include "Intmash_bastypes.h"//содержит описания базовых типов
///////////////////////////////////////////////////////////////////////////////
typedef struct  {
    tU32 SerialNumber;//0,1 
 } TCD_DATA;

//определение диапазона доступных регистров
#define CD_DATA_PREFIX 0xC
#define CD_DATA_SIZE 0xee
////////////////////////////////////////////////////////////////////////////////
//параметры расположеные  в CD
extern const TCD_DATA CD_DATA;



