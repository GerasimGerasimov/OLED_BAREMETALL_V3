#ifndef FLOAT_TYPE_SIGNAL_H
#define FLOAT_TYPE_SIGNAL_H

#include "stm32f4xx.h"
#include "generalcase.h"

class TFloat : public TGeneralCaseSignal {
public:
	TFloat(TSignalPropsPointers props);
	~TFloat();
private:
	TGenaralCaseRawReturn getRawValue(const TSlotHandlerArsg& args);
};

#endif