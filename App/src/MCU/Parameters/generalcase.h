#ifndef GENERAL_CASE_H
#define GENERAL_CASE_H

#include "stm32f4xx.h"
#include <vector>
#include <string>
#include "parameters.h"

enum class TGeneralCaseReturnType : u8
{
	U = 0,
	S = 1,
	F = 2
};

typedef struct {
	TGeneralCaseReturnType type;
	union {
		float f;
		u32 i;
		s32 s;
	} raw;
} TGenaralCaseRawReturn;


class TGeneralCaseSignal : public TParameter{
public:
	TGeneralCaseSignal(TSignalPropsPointers props);
	~TGeneralCaseSignal();
	std::string getMSU(); 
	virtual std::string getValue(const TSlotHandlerArsg& args, const char* format);
	virtual void setValue();
protected:
	std::string value(const TSlotHandlerArsg& args, const char* format);
	virtual TGenaralCaseRawReturn getRawValue(const TSlotHandlerArsg& args);
	std::string validation(const TSlotHandlerArsg& args);
	char* MSU;
	s16 Addr;
	float Scale;
};

#endif