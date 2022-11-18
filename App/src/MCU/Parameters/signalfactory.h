#ifndef SIGNAL_FACTORY_H
#define SIGNAL_FACTORY_H

#include "stm32f4xx.h"
#include "signal.h"
#include "vars.h"
#include "parameters.h"
#include <vector>
#include <string>

class SignalFactoty {
public:
	static TSignalPropsPointers getSignalProps(const char* dev, const char* source, const int srcLen);
	static TScaleProps getScaleProps(const char* source, const int srcLen);
	static pSignal getSignal(const TSignalPropsPointers& props);
	static pSignal getScale(const TScaleProps& props);
};

#endif