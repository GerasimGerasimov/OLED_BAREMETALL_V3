#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <string>
#include <vector>
#include "stm32f4xx.h"
#include "TMCUText.h"

#define VIEW_PORT_MAX_WIDTH  127 //ширина
#define VIEW_PORT_MAX_HEIGHT 63  //высота

#define PRIMARY_COLOR  1
#define BACKGROUND_COLOR 0

struct TPixel {
	s16 x;
	s16 y;
	u8 color;
};

struct TFillRect {
	s16 left;
	s16 top;
	u16 width;
	u16 height;
	u16 color;
};

struct TClipRect {
	s16 left;
	s16 top;
	u16 width;
	u16 height;
};

struct TGraphics {
	public:
		static void init(void);
		static void fillRect(TFillRect props);
		static void Line(u8 X1, u8 Y1, u8 X2, u8 Y2, u8 Color);
		static void setPixel(TPixel& props);
		static void setPixel(u8 x, u8 y, u8 color);
		static void outText(std::string text, s16 x, s16 y, u16 color, std::string FontName);
		static void putChar(u8 Code, s16& x, s16 y, u16 color);
		static void outTextClipped(std::string text, u16 x, u16 y, u16 color, std::string FontName, TClipRect& rect);
		static void putTextWithSelectedChar(u8* src, u8 len, s16& x, s16 y, u8 Selected, u16 BaseColor, u16 SelectColor);
		static u8 screen[128][64];
	private:
		static void putCharClipped(u8 Code, u16 & x, u16 y, u16 color, TClipRect& rect);
};

#endif
