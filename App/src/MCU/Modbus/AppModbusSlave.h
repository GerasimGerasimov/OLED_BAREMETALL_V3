#include "crc16.h"
#include "bastypes.h"
#include <string>
#include <map>
#include <functional>
#include <Slot.h>

typedef struct {
	const std::string& Cmd;
	const std::string& DevAddr;
	const std::string& RegAddr;
	const std::string& Value;
} TWriteCmdSrc;

class ModbusSlave {
	using TSlotDataHandler = std::function<void(Slot& slot, u8* reply)>;
public:
	static bool setValue(std::string& tag, std::string& value, TSlotDataHandler callback);
private:
	static u8 CreateWriteCmd(u8* a, TWriteCmdSrc CmdSrc);
	static u8 get0x10WriteRegCmd(u8* a, TWriteCmdSrc& Src);
	static std::string NetWorkAddrToHex(u16 nwa);
	static const std::map < std::string, std::function <u8(u8*, TWriteCmdSrc&) >> WriteCmdVariants;
};