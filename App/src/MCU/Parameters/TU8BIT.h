#ifndef USBYTE_TYPE_SIGNAL_H
#define USBYNE_TYPE_SIGNAL_H

#include "stm32f4xx.h"
#include "generalcase.h"

class TU8BIT : public TParameter {
public:
	TU8BIT(TSignalPropsPointers props);
	~TU8BIT();
	std::string getMSU();
	virtual std::string getValue(const TSlotHandlerArsg& args, const char* format);
    std::string getValue(){return "";};
	const std::string getValueHex(std::string& src);
	const std::string getRegHexAddr();
	const std::string getWriteCmdType();
	virtual void setValue();
	const std::string& getSignalType();
	TInternalMemAddress getInternalMemAddr();
private:
	char* MSU;
	float Scale;
	TSpecialSignalAddr Addr;
	const std::string value(const TSlotHandlerArsg& args, const char* format);
	u8 getRawValue(const TSlotHandlerArsg& args);
	const std::string validation(const TSlotHandlerArsg& args);
	u16 string2raw(std::string& src);
};

#endif