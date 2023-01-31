#ifndef SET_VALUE_SELECTOR_H
#define SET_VALUE_SELECTOR_H

#include "AppModbusSlave.h"
#include "bastypes.h"
#include <string>
#include <functional>
#include <Slot.h>

class SetValueSelector {
using TSlotDataHandler = std::function<void(Slot* slot, u8* reply)>;
public:
	static bool setValue(std::string& tag, std::string& value, TSlotDataHandler callback);
private:
	static bool setInternalValue(std::string& tag, std::string& value);
};

#endif