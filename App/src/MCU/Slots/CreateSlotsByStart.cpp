#include "CreateSlotsByStart.h"
//#include "CommonSlotHandler.h"

//static const u8 cmdGetID[] = { 1,    3, 0x00, 0x00, 0x00, 0x1F };
//                               adr,cmd, firstAddr , regsToRead
const std::vector<u8> getReadCmdFromSettings(u16 NetWorkAddr, u16 firstAddr, u16 lastAddr) {
	u16 regsToRead = lastAddr - firstAddr;
	std::vector<u8> res = {(u8)NetWorkAddr, 0x03,
		                   (u8)((firstAddr >> 8) & 0xff), (u8) (firstAddr & 0xff),
		                   (u8)((regsToRead >> 8) & 0xff), (u8) (regsToRead & 0xff)
	                      };
	return res;
}

std::vector <Slot> CreateSlotsByStart::init(std::map<std::string, TDeviceNetworkProps> Devices) {
	std::vector <Slot> res;
	for (auto& device : Devices) {
		std::string devname = device.first;
		TDeviceNetworkProps slotsprops = device.second;
		for (auto& slotprop : slotsprops.SlotsMap) {
			std::string section = slotprop.first;
			TDeviceSlotProps* prop = slotprop.second;
			/*TODO собственно создать слот для заданного COM порта*/
			Slot* pSlot = new Slot(devname, section, prop->StartAddr, prop->LastAddr);
			pSlot->addcmd(getReadCmdFromSettings(slotsprops.NetworkAddr, prop->StartAddr, prop->LastAddr));
			pSlot->onData = nullptr;//CommonSlotHandler::parseRespond;
			pSlot->Interval = prop->Interval;
			res.push_back(*pSlot);
		}
	}
	return res;
}

