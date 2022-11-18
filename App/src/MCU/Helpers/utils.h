#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "stm32f4xx.h"
#include "bastypes.h"

class Utils {
public:
	//std::wstring s2ws(const std::string& str);
	static void swp_copy_u16(u8* s, u16* d, u8 c);
	static void comma_to_dot(char* input);
	static std::string getValueAsFormatStr(float& value, const char* format);
};

#endif