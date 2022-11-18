#ifndef WORD_TYPE_SIGNAL_H
#define WORD_TYPE_SIGNAL_H

#include "stm32f4xx.h"
#include "generalcase.h"
#include <string>

class TU16BIT : public TGeneralCaseSignal {
public:
	TU16BIT(TSignalPropsPointers props);
	~TU16BIT();
	const std::string getValueHex(std::string& src);
	const std::string getRegHexAddr();
	const std::string getWriteCmdType();
private:
	virtual TGenaralCaseRawReturn getRawValue(const TSlotHandlerArsg& args);
	u16 string2raw(std::string& src);
};

#endif