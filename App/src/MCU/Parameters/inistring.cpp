#include "inistring.h"
#include "vars.h"
#include "parameters.h"
#include "signalfactory.h"

pSignal TIniString::getSignal(const std::string& dev, const std::string& section, char* source, int scrLen) {
	TSignalPropsPointers props = SignalFactoty::getSignalProps(dev.c_str(), source, scrLen);
	pSignal s = SignalFactoty::getSignal(props);
	return s;
}

pSignal TIniString::getScale(std::string section, char* source, int scrLen) {
	TScaleProps props = SignalFactoty::getScaleProps(source, scrLen);
	pSignal s = SignalFactoty::getScale(props);
	return s;
}

