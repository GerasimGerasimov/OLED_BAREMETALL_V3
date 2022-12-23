#ifndef STATE_CONTROLLERS_H
#define STATE_CONTROLLERS_H

#include "stm32f4xx.h"
#include <string>
#include "TBit.h"

typedef struct TTrackedBit {
	const std::string Tag;
	TBit* pBit;
	bool isValid;
	bool State;
};

#endif