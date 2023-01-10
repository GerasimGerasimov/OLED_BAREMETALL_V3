#include "DevicePollManager.h"
#include "crc16.h"
//#include "consolelog.h"

//#include "U1RamUpdate.h"

//static U1RAMSlot SlotU1RAM;

std::vector <Slot> DevicePollManager::Slots = {};
Slot* DevicePollManager::slot = nullptr;
u16 DevicePollManager::idx = 0;
u16 DevicePollManager::Status = (u16)DevicePollManagerStatus::TOGGLE_SLOT;
u8 DevicePollManager::Reply[256] = {};
s16 DevicePollManager::ReplyResult = 0;

void DevicePollManager::init(std::vector <Slot> slots) {
	//SlotU1RAM.init();
	Slots = slots;//{ SlotU1RAM };
}

void DevicePollManager::execute(void) {
	switch ((DevicePollManagerStatus)Status)
	{
	case DevicePollManagerStatus::SEND_REQUEST:
		ComMasterDriver::send({ (u8*)&slot->OutBuf, slot->cmdLen, slot->TimeOut, {checkRespond} });
		Status = (u16)DevicePollManagerStatus::WAIT_RESPOND;
		break;
	case DevicePollManagerStatus::WAIT_RESPOND:
		break;
	case DevicePollManagerStatus::PARSE_RESPOND:
		slot->validation(ReplyResult, (u8*) &Reply);
 		Status = (u16)DevicePollManagerStatus::TOGGLE_SLOT;
		break;
	case DevicePollManagerStatus::TOGGLE_SLOT:
		slot = getNextSlot();
		Status = (u16)setActionBySlot();
		break;
	default:
		break;
	}
}

DevicePollManagerStatus DevicePollManager::setActionBySlot(void) {
	if (slot == NULL) return DevicePollManagerStatus::TOGGLE_SLOT;

	if (slot->Flags & (u16)SlotStateFlags::SKIP_SLOT) {
		return DevicePollManagerStatus::TOGGLE_SLOT;
	}
	else {
		return (slot->isIntervalDone())
			? DevicePollManagerStatus::SEND_REQUEST
			: DevicePollManagerStatus::TOGGLE_SLOT;
	}
}

Slot* DevicePollManager::getNextSlot(void) {
	u16 size = Slots.size();//1 и больше если в списке что-то есть
	u16 tmp_idx = idx;
	if (size > 0) {
		idx = (++tmp_idx >= size)
			? 0
			: tmp_idx;
		Slot* s = &Slots[idx];
		return s;
	}
	else {
		return nullptr;
	}
}

void DevicePollManager::checkRespond(s16 result, u8* reply) {
	if (result > 0) {
		std::memcpy(Reply, reply, result);
	} 
	ReplyResult = result;
	Status = (u16)DevicePollManagerStatus::PARSE_RESPOND;
}

Slot* DevicePollManager::getSlotByDevPosAndSection(const std::string& device, const std::string& section) {
	for (auto& s : Slots) {
		if ((s.Device == device) && (s.Section == section)) {
			return &s;
		}
	}
	return nullptr;
}