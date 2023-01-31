#include "SetValueSelector.h"
#include "IniResources.h"
#include <parameters.h>
#include "MemWrite.h"

/*Данные могут размещаться во внутренней памяти МК, или на внешнем устройсве
Если на внешнем устройсве, будет вызываться ModbusSlave::setValue,
если во врутренней памяти МК то другая функция
А определяется это через tag
Если название устройсва "SLF" значит память МК*/
bool SetValueSelector::setValue(std::string& tag, std::string& value, TSlotDataHandler callback) {
	const TValueSearchStruct Info = IniResources::spliceTagInfo(tag);
	if (Info.device == "SLF") {
		return SetValueSelector::setInternalValue(tag, value);
	}
	else {
		return ModbusSlave::setValue(tag, value, callback);
	}
}

bool SetValueSelector::setInternalValue(std::string& tag, std::string& value) {
	const TValueSearchStruct Info = IniResources::spliceTagInfo(tag);
	TParameter* p = (TParameter*)IniResources::getSignalByTag(tag);
	std::string ValueHex = p->getValueHex(value);//получил значение в хексах и сразу длину (так как строка)
	TInternalMemAddress Addr = p->getInternalMemAddr();
	//уже понятно что это SLF
	if (Info.section == "FLASH") {
		MemWrite::writeValueToFlash(Addr, ValueHex);
	} else if (Info.section == "CD") {
		MemWrite::writeValueToCD(Addr, ValueHex);
        }
	return false;
}