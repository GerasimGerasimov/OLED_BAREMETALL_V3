#ifndef CREATE_SLOTS_BY_START_H
#define CREATE_SLOTS_BY_START_H
#include "IniSlotsProps.h"
#include "Slot.h"

class CreateSlotsByStart {
public:
	static std::vector <Slot> init(std::map<std::string, TDeviceNetworkProps> Devices);
};

#endif