#include "com_data_valid.h"
#include "stm32f4xx.h"

#define RAM_DATA_VALID_COUNT 3 //3 безошибочных чтения данных и они становятся валидными 
static bool RamDataValid = false;
static int RamDataValidCount = RAM_DATA_VALID_COUNT;

bool isRAMDataValid(void){
  return RamDataValid;
}

void setRamDataValidState(void){
  if (RamDataValidCount) {
    RamDataValidCount--;
  } else {
    RamDataValid = true;
  }
}

void resetRamDataValid(void){
  RamDataValid = false;
  RamDataValidCount = RAM_DATA_VALID_COUNT;
}