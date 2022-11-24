#ifndef ISIGNAL_H
#define ISIGNAL_H

#include "stm32f4xx.h"
#include <vector>
#include <string>
#include "SlotHandlerType.h"

typedef struct {
	char* dev;
	char* pName;
	char* pComment;
	char* pType;
	char* pOptional;
} TSignalPropsPointers;


class ISignal {
public:
	ISignal(char* source, int scrLen);
	ISignal(TSignalPropsPointers props);
	~ISignal();
	virtual std::string getName();
	virtual std::string getValue(const TSlotHandlerArsg& args, const char* format);
        virtual std::string getValue(){return "";};
	virtual std::string getMSU();
	std::string getDefaultValue();
protected:
	char* Name;
	char* optional;
};

typedef ISignal* pSignal;

#endif