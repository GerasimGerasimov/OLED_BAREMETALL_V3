#ifndef PARAMETERS_UTILS_H
#define PARAMETERS_UTILS_H

#include "stm32f4xx.h"
#include <vector>
#include <string>
#include "parameters.h"

class ParametersUtils {
public:
	static s16 getByteOffsetFromSlahedAddrStr(char* addr);
	static TSpecialSignalAddr getSpecialAddrForBit(char* addr);
	static TSpecialSignalAddr getSpecialAddrForByte(char* addr);
	static bool isAddrInvalid(s16 Addr);
	static bool isAddrValid(s16 Addr);
};

#endif