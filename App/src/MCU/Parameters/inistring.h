#ifndef INISTRING_H
#define INISTRING_H

#include "stm32f4xx.h"
#include <vector>
#include <string>
#include "signal.h"

class TIniString {
public:
	static pSignal getSignal(const std::string& dev, const std::string& section, char* source, int scrLen);
	static pSignal getScale(std::string section, char* source, int scrLen);
};

#endif