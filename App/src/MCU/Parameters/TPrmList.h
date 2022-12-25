#ifndef PRMLIST_TYPE_SIGNAL_H
#define PRMLIST_TYPE_SIGNAL_H

#include "stm32f4xx.h"
#include "generalcase.h"

class TPrmList : public TParameter {
public:
	TPrmList(TSignalPropsPointers props);
	~TPrmList();
	std::string getMSU();
	virtual std::string getValue(const TSlotHandlerArsg& args, const char* format);
        std::string getValue(){return "";};
	virtual void setValue();
private:
	char* MSU;
	float Scale;
	TSpecialSignalAddr Addr;
	const std::string value(const TSlotHandlerArsg& args, const char* format);
	u8 getRawValue(const TSlotHandlerArsg& args);
	const std::string validation(const TSlotHandlerArsg& args);
};

#endif