#include "TMCUText.h"

pMCUFont TMCUText::font = NULL;
u8 TMCUText::BytesByWidth = 0;
u8 TMCUText::index = 0;
u8* TMCUText::CharImage = NULL;
u16 TMCUText::ImageSize = 0;
u8 TMCUText::ImageHeight = 0;

u8 TMCUText::setFont(std::string name) {
	font = TMCUFonts::getFont(name);
	ImageHeight = font->height;
	return ImageHeight;
}

TCharProps TMCUText::setSimbol(u8 Code) {
	u16 width = font->chars[Code].width;
	CharImage = (u8 *)font->chars[Code].image;
	index = 0;
	BytesByWidth = (width / 8) + ((width % 8 == 0) ? 0 : 1);
	ImageSize = BytesByWidth * ImageHeight;
	return { BytesByWidth , width };
}

bool TMCUText::getBitsLine(u32& bits) {
	if (index != ImageSize) {
		bits = 0;
		u8 i = BytesByWidth;
		while (i-- != 0) {
			bits = bits << 8;
			bits |= CharImage[index++];
		}
		return true;
	}
	else
		return false;
}
