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
	std::vector<std::string> getList(void);
	std::vector<std::string> getList(const std::string& val, s16& ValueIndexInList);
	std::string getKeyByValue(const std::string& val);
	virtual void setValue();
	const std::string getValueHex(std::string& src);
	const std::string getRegHexAddr();
	const std::string getWriteCmdType();
	const std::string& getSignalType();
	TInternalMemAddress getInternalMemAddr();
private:
	char* MSU;
	float Scale;
	TSpecialSignalAddr Addr;
	const std::string value(const TSlotHandlerArsg& args, const char* format);
	u8 getRawValue(const TSlotHandlerArsg& args);
	const std::string validation(const TSlotHandlerArsg& args);
};

#endif