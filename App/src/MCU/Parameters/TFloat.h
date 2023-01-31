#ifndef FLOAT_TYPE_SIGNAL_H
#define FLOAT_TYPE_SIGNAL_H

#include "stm32f4xx.h"
#include "generalcase.h"

class TFloat : public TGeneralCaseSignal {
public:
	TInternalMemAddress getInternalMemAddr();
	const std::string getValueHex(std::string& src);
	TFloat(TSignalPropsPointers props);
	~TFloat();
private:
	std::string value(const TSlotHandlerArsg& args, const char* format);
	TGenaralCaseRawReturn getRawValue(const TSlotHandlerArsg& args);
	u32 string2raw(std::string& src);
};

#endif