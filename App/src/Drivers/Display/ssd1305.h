#ifndef _SSD1305_H
#define _SSD1305_H

#define SSD1305_CMD_SWRES          0xe2
#define SSD1305_CMD_SETLOWCOLADD   0x00
#define SSD1305_CMD_SETHIGHCOLADD  0x10
#define SSD1305_CMD_SETMEMADDMODE  0x20
#define SSD1305_CMD_SETCOLADD      0x21
#define SSD1305_CMD_SETPAGEADD     0x22
#define SSD1305_CMD_SETDISPSTART   0x40
#define SSD1305_CMD_SETCONTRAST    0x81
#define SSD1305_CMD_SETCBRIGHTNESS 0x82
#define SSD1305_CMD_SETCLUT        0x91
#define SSD1305_CMD_SETP0COLR      0x92
#define SSD1305_CMD_SETP1COLR      0x93
#define SSD1305_CMD_SETSEGREMAP    0xa0

#define SSD1305_CMD_ALLDISPON      0xa4
#define SSD1305_CMD_NORMALINV      0xa6
#define SSD1305_CMD_SETMUXRATIO    0xa8
#define SSD1305_CMD_DIMMODESETTING 0xab
#define SSD1305_CMD_DISPOFF        0xae
#define SSD1305_CMD_DISPDIM        0xac
#define SSD1305_CMD_DISPON         0xaf

#define SSD1305_CMD_SETPAGEADD_PGMODE     0xb0
#define SSD1305_CMD_SETSCANDIR            0xc0
#define SSD1305_CMD_SETDISPOFFSET         0xd3
#define SSD1305_CMD_SETCLKDIV             0xd5
#define SSD1305_CMD_SETAREACOLRMODE       0xd8
#define SSD1305_CMD_SETPRECHARGE          0xd9
#define SSD1305_CMD_SETHWCONFIG           0xda
#define SSD1305_CMD_SETRGHTHORIZSCROLL    0x29
#define SSD1305_CMD_SETVERLEFTHORIZSCROLL 0x2a
#define SSD1305_CMD_SETVERTSCROLLAREA     0xA3
#define SSD1305_CMD_HORIZSCROLLEN         0x2f
#define SSD1305_CMD_HORIZSCROLLDIS        0x2e
#define SSD1305_CMD_VCOMLEVEL             0xdb
#define SSD1305_CMD_MASTERCONFIG          0xad

  #ifdef __cplusplus
    extern "C" void ssd1305_init();
    extern "C" void ssd1305_rst_first();
    extern "C" void ssd1305_rst_second();
  #else
    extern void ssd1305_init();
    extern void ssd1305_rst_first();
    extern void ssd1305_rst_second();
  #endif

// #RES pin action
#define LCD_RES_LO GPIO_ResetBits(GPIOB, GPIO_Pin_0)
#define LCD_RES_HI GPIO_SetBits(GPIOB, GPIO_Pin_0)

// D/#C pin action
#define LCD_COMMAND GPIO_ResetBits(GPIOA, GPIO_Pin_6)
#define LCD_DATA    GPIO_SetBits(GPIOA, GPIO_Pin_6)

// NSS pin action
#define LCD_NSS_LO GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define LCD_NSS_HI GPIO_SetBits(GPIOA, GPIO_Pin_4)

#endif // _SSD1305_H