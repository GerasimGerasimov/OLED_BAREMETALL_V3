#ifndef BIT_TYPE_SIGNAL_H
#define BIT_TYPE_SIGNAL_H

#include "stm32f4xx.h"
#include "parameters.h"

class TBit : public TParameter {
public:
	TBit(TSignalPropsPointers props);
	~TBit();
	virtual std::string getValue(const TSlotHandlerArsg& args, const char* format);
private:
	TSpecialSignalAddr Addr;
	const std::string value(const TSlotHandlerArsg& args);
	bool getRawValue(const TSlotHandlerArsg& args);
	const std::string validation(const TSlotHandlerArsg& args);
};

#endif