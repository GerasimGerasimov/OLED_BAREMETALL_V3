#ifndef EMB_FONTS_H
#define EMB_FONTS_H

#include <stm32f4xx.h>

 typedef struct {
     const u8 code;
     const u8* image;
     const u8 width;
     } tChar;

 typedef struct {
     const u16 length;
     const u8 height;
     const tChar *chars;
     } tFont;

#endif