#include "U1RamUpdate.h"
#include "consolelog.h"
#include "resources.h"
#include "utils.h"

static const u8 cmdGetID[] = { 1, 3, 0x00, 0x00, 0x00, 0x1F };

void U1RAMSlot::init(void) {
	Flags = 0;
	addcmd((u8*)cmdGetID, sizeof(cmdGetID));
	onData = [this](Slot &slot, u8* reply) { parceRespond(slot, reply); };
}

void U1RAMSlot::parceRespond(Slot& slot, u8* reply) {
	if (slot.RespondLenghtOrErrorCode == 0) {
		//console::log(L"comcallback:Error\n");
		setParametersUnnown();
		//std::string str = TInternalResources::getID();
		//str += "\n";
		//std::wstring wstr(str.begin(), str.end());
		//console::log(wstr);
	}
	else {
		getPuretDataFromRespond(slot.RespondLenghtOrErrorCode, slot.InputBuf);
		//setParametersValue(result, reply);
		//std::string str((char*)reply, (int)result);
		//str += "\n";
		//std::wstring wstr(str.begin(), str.end());
		//console::log(wstr);
	}
}

void  U1RAMSlot::getPuretDataFromRespond(s16 result, u8* reply) {
	u8 msg_count = reply[2] >> 1;
	Utils::swp_copy_u16((u8*) reply[3],
		(u16*)&InputBuf,
		msg_count);
}

void U1RAMSlot::setParametersValue(s16 result, u8* reply) {

}

void U1RAMSlot::setParametersUnnown(void) {

}

U1RAMSlot::U1RAMSlot() {

}

U1RAMSlot::~U1RAMSlot() {

}