#ifndef BUTTONS_H
#define BUTTONS_H
/*
#define F1_butt 1       //1 
#define F2_butt 2      //2 F2
#define F3_butt 4        //3 F3
#define Esc_butt 8      //4 
#define Up_butt 16      //5 
#define Ent_butt 32     //6 
#define Left_butt 64    //7 
#define Down_butt 128   //8
#define Right_butt 256  //9 
*/
  #ifdef __cplusplus
    extern "C" void KeyBoardUpdate (void);
  #else
    extern void KeyBoardUpdate (void);
  #endif

#endif