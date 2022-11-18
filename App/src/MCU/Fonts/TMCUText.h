#ifndef T_MCU_TEXT_H
#define T_MCU_TEXT_H

#include "TMCUFonts.h"
#include "stm32f4xx.h"

typedef struct {
	const u16 BytesByWidth;
	const u16 BitsByWidth;
} TCharProps;

struct TMCUText {
public:
	static u8 setFont(std::string name);
	static TCharProps setSimbol(u8 Code);
	static bool getBitsLine(u32 &bits);
private:
	static pMCUFont font;
	static u8 BytesByWidth;
	static u16 ImageSize;
	static u8 ImageHeight;
	static u8 index;
	static u8* CharImage;
};

#endif