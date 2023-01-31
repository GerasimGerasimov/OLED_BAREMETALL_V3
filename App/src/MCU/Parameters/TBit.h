#ifndef BIT_TYPE_SIGNAL_H
#define BIT_TYPE_SIGNAL_H

#include "stm32f4xx.h"
#include "parameters.h"

class TBit : public TParameter {
public:
	TBit(TSignalPropsPointers props);
	~TBit();
	virtual std::string getValue(const TSlotHandlerArsg& args, const char* format);
    std::string getValue(){return "";};
	const std::string getValueHex(std::string& src);
	const std::string getRegHexAddr();
	const std::string getWriteCmdType();
	const std::string& getSignalType();
	TInternalMemAddress getInternalMemAddr();
private:
	TSpecialSignalAddr Addr;
	const std::string value(const TSlotHandlerArsg& args);
	bool getRawValue(const TSlotHandlerArsg& args);
	const std::string validation(const TSlotHandlerArsg& args);
};

#endif