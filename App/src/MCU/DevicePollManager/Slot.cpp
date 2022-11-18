#include "Slot.h"
#include "crc16.h"

Slot::Slot()
	: Flags(0)
	, RespondLenghtOrErrorCode(0)
	, onData(nullptr)
	, cmdLen(0)
	, StartAddrOffset(0)
	, LastAddrOffset(0)
	, Interval(0)
	, TmpInterval(0)
	, InputBufValidBytes(0)
	, DelayAfterWrite(0){
}

Slot::Slot(std::string device, std::string section, u16 StartAddr, u16 LastAddr)
	: Flags(0)
	, RespondLenghtOrErrorCode(0)
	, onData(nullptr)
	, cmdLen(0)
	, Device(device)
	, Section(section)
	, StartAddrOffset((StartAddr & 0x00FF) << 1)
	, LastAddrOffset((LastAddr & 0x00FF) << 1)
	, Interval(0)
	, TmpInterval(0)
	, InputBufValidBytes(0)
	, DelayAfterWrite(0)
{

}

void Slot::init(void) {

}

void Slot::addcmd(u8 cmd[], u8 size) {
	std::memcpy(OutBuf, cmd, size);
	cmdLen = size + 2;
	FrameEndCrc16((u8*)&OutBuf, cmdLen);
}


void Slot::addcmd(const std::vector<u8>& v) {
	//GIST копирование вектора в массив
	std::memcpy(OutBuf, v.data(), v.size());
	cmdLen = v.size() + 2;
	FrameEndCrc16((u8*)&OutBuf, cmdLen);
}

bool Slot::isReplyCRCValid(s16 result, u8* reply) {
	return (result <= 0)
		   ? false
		   : (bool)(crc16(reply, result) == 0);
}

void Slot::validation(s16 result, u8* reply) {
	(isReplyCRCValid(result, reply))
		? (Flags &= ~(u16)SlotStateFlags::CRC_ERR)
		: (Flags |=  (u16)SlotStateFlags::CRC_ERR);
	RespondLenghtOrErrorCode = result;
	if (onData)
		onData(*this, reply);
}

bool Slot::isIntervalDone() {
	if (++TmpInterval > Interval) {
		TmpInterval = 0;
		return true;
	}
	return false;
}

Slot::~Slot() {}

