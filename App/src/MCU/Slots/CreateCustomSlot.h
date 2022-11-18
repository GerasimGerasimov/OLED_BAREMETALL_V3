#ifndef CREATE_CUSTOM_SLOT_H
#define CREATE_CUSTOM_SLOT_H
#include "IniSlotsProps.h"
#include "Slot.h"

class CreateCustomSlot {
public:
	static Slot* init(std::string devname, std::string section);
};

#endif