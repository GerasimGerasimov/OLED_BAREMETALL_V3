#include "CreateCustomSlot.h"
#include "CommonSlotHandler.h"

Slot* CreateCustomSlot::init(std::string devname, std::string section) {
	Slot* res = new Slot(devname, section, 0, 0);
        res->Flags |= (u16)SlotStateFlags::SKIP_SLOT;
	res->DelayAfterWrite = 2000;
	return res;
}

