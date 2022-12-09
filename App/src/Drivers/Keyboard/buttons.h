#ifndef BUTTONS_H
#define BUTTONS_H

#include "stm32f4xx.h"
//пленочная клавиатура

#define F1_butt 1       //1 
#define F2_butt 2      //2 F2
#define F3_butt 4        //3 F3
#define Esc_butt 8      //4 
#define Up_butt 16      //5 
#define Ent_butt 32     //6 
#define Left_butt 64    //7 
#define Down_butt 128   //8
#define Right_butt 256  //9 


#define ROW1 GPIO_Pin_0
#define ROW2 GPIO_Pin_1
#define ROW3 GPIO_Pin_2
#define COL1 GPIO_Pin_13
#define COL2 GPIO_Pin_14
#define COL3 GPIO_Pin_15
#define COLUMNS 0xe000//0x38    //
#define COLUMNS_OFS 0x07 
#define ROWS 0x07

  #ifdef __cplusplus
    extern "C" u16 buttons_check (void);
    extern "C" void buttons_scan(u16 btn_tmp);
  #else
    extern u16 buttons_check (void);
    extern void buttons_scan(u16 btn_tmp);
  #endif



#endif