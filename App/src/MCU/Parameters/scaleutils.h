#ifndef SCALE_UTILS_H
#define SCALE_UTILS_H

#include "stm32f4xx.h"
#include <string>

class ScaleUtils {
public:
	static float getScaleFromProps(const char* dev, const char* optional);
private:
	static std::string getVarStrValueByKey(std::string& key, const char* dev);
};

#endif