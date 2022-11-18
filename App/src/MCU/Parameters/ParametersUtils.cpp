#include "ParametersUtils.h"
#include <parser.h>

s16 ParametersUtils::getByteOffsetFromSlahedAddrStr(char* addr) {
	//отрицательное значение - значит адрес не декодирован!
	//используется младшая часть адреса для получения смещения в байтах
	//т.е. адрес = отрасывается префикс, и младшая часть умножается на 2
	u16 res = 0;
	if (addr) {
		addr++; //приходит строка в формате rAAAA, тут передвигаюсь с"r" на следующий символ для правильного декодирования
		std::string s = IniParser::getElement('/', addr);
		try {
			res = std::stoi(s, 0, 16);
			res = (res & 0x00FF) * 2;
		}
		catch (...) {
			res = -1;
		}
	}
	return (s16)res;
}

/*В таком виде r2021.1/ приходит адрес, надо получить смешение и маску для бита*/
TSpecialSignalAddr ParametersUtils::getSpecialAddrForBit(char* addr) {
	//отрицательное значение - значит адрес не декодирован!
	//используется младшая часть адреса для получения смещения в байтах
	//т.е. адрес = отрасывается префикс, и младшая часть умножается на 2
	if (addr) {
		try {
			addr++; //приходит строка в формате rAAAA.N, тут передвигаюсь с"r" на следующий символ для правильного декодирования
			std::string s = IniParser::getElement('.', addr);
			s16 offset = std::stoi(s, 0, 16);
			offset = (offset & 0x00FF) * 2;
			addr +=5; //перехожу к N
			s = IniParser::getElement('/', addr);
			u16 bitnum = std::stoi(s, 0, 16);
			u16 mask  = 1 << bitnum;
			return { offset, mask };
		}
		catch (...) {
			return { -1, 0 };
		}
	}
	return { -1, 0 };
}

/*В таком виде r2021.H(L)/ приходит адрес, надо получить смешение и номер байта*/
TSpecialSignalAddr ParametersUtils::getSpecialAddrForByte(char* addr) {
	//отрицательное значение - значит адрес не декодирован!
	//используется младшая часть адреса для получения смещения в байтах
	//т.е. адрес = отрасывается префикс, и младшая часть умножается на 2
	if (addr) {
		try {
			addr++; //приходит строка в формате rAAAA.H(L), тут передвигаюсь с"r" на следующий символ для правильного декодирования
			std::string s = IniParser::getElement('.', addr);
			s16 offset = std::stoi(s, 0, 16);
			offset = (offset & 0x00FF) * 2;
			addr += 5; //перехожу к H/L
			s = IniParser::getElement('/', addr);
			u16 bytenum = (s == "H")
				? 1 : 0;
			return { offset, bytenum };
		}
		catch (...) {
			return { -1, 0 };
		}
	}
	return { -1, 0 };
}

bool ParametersUtils::isAddrInvalid(s16 Addr) {
	return (bool)(Addr < 0);
}

bool ParametersUtils::isAddrValid(s16 Addr) {
	return (bool)(Addr >= 0);
}