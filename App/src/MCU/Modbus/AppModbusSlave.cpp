#include "AppModbusSlave.h"
#include <parameters.h>
#include <IniResources.h>
#include <DevicePollManager.h>

void ModbusSlave::setValue(std::string& tag, std::string& value, TSlotDataHandler callback) {
    TParameter* p = (TParameter*)IniResources::getSignalByTag(tag);
    std::string ValueHex = p->getValueHex(value);//получил значение в хексах и сразу длину (так как строка)
    std::string RegHexAddr = p->getRegHexAddr();//получил номер регистра в хексах
    const std::string Cmd = p->getWriteCmdType();//тип команды записи 10 или 16
    /*по Tag надо узнаю на какой Адрес отправлять
        "DEVICES":"U1/", есть список устройств, надо сопостоавить из тега U1
    "U1":"DEV1/COM1/1 а из U1 найти адрес (он за COM1/)*/
    const u16 DevAddr = IniResources::getDevNetWorkAddrByTag(tag);
    const std::string DevAddrHex = ModbusSlave::NetWorkAddrToHex(DevAddr);
    const std::string DevPos = IniResources::getDevicePositionByTag(tag);
    const std::string Section = "CmdWrite";
    Slot* slot = DevicePollManager::getSlotByDevPosAndSection(DevPos, Section);
    slot->cmdLen = ModbusSlave::CreateWriteCmd(slot->OutBuf, { Cmd, DevAddrHex, RegHexAddr, ValueHex });
    slot->onData = callback;
    slot->Flags &= ~(u16)SlotStateFlags::SKIP_SLOT;
}

const std::map < std::string, std::function <u8(u8*, TWriteCmdSrc&) >> ModbusSlave::WriteCmdVariants = {
    {"10", [](u8* a, TWriteCmdSrc& props) {return get0x10WriteRegCmd(a, props); }},
    {"16", [](u8* a, TWriteCmdSrc& props) {return 0; }},
};

u8 ModbusSlave::CreateWriteCmd(u8* a, TWriteCmdSrc CmdSrc) {
    u8 res = (WriteCmdVariants.count(CmdSrc.Cmd))
        ? WriteCmdVariants.at(CmdSrc.Cmd)(a, CmdSrc)
        : 0;
    return res;
}

/*запрос 0x10 на выходе
AD, C, №R(h, l), CW(h, l), CntBytes, Data[0](h, l)..Data[CW](h, l), CRC(h, l)
│   │  │          │          │       │             │                └─(word)crc16
│   │  │          │          │       └─────────────┴─(word)Записываемые данные
│   │  │          │          └─(byte)Кол - во передаваемых байт
│   │  │          └─(word)Кол - во регистров к передаче
│   │  └─(word)Начальный адрес
│   └────(byte)Команда(10h)
└──────(byte)Адрес устройства
*/

u8 ModbusSlave::get0x10WriteRegCmd(u8* a, TWriteCmdSrc& Src) {
    a[0] = (u8)std::stoi(Src.DevAddr);
    a[1] = 0x10;
    //начальный адрес регистра
    u16 addr = (u16)std::stoul(Src.RegAddr, nullptr, 16);
    a[2] = (u8)(addr >> 8) & 0x00FF;
    a[3] = (u8)(addr & 0x00FF);
    //кол-во передаваемых регистров
    u8 regscnt = (u8)(Src.Value.size() / 4);
    a[4] = 0;
    a[5] = regscnt;
    a[6] = regscnt * 2;//кол-во передаваемых байт
    //данные
    u32 value = std::stol(Src.Value, nullptr, 16);
    (regscnt == 1)
        ? (a[7] = (value & 0x0000FF00) >> 8,
            a[8] = (value & 0x000000FF) >> 0)
        : (a[7] = (value & 0xFF000000) >> 24,
            a[8] = (value & 0x00FF0000) >> 16,
            a[9] = (value & 0x0000FF00) >> 8,
            a[10] = (value & 0x000000FF) >> 0);
    //контрольная сумма 
    u8 cmdLen = (regscnt == 1) ? 11 : 13;
    FrameEndCrc16(a, cmdLen);
    return cmdLen;
}

std::string ModbusSlave::NetWorkAddrToHex(u16 nwa) {
    char s[4];
    sprintf(s, "%.2X", nwa);
    std::string res(s);
    return res;
}

/*
// rC004 -> 0xC004 (49156)
u16 ModbusSlave::NumericRegAddrToU16(char* RegAddr) {
	std::string s(++RegAddr, 4);
	//GIST преобразование HEX-строки в беззнаковое целое число, основание 16 для HEX
	u16 res = std::stoul(s, nullptr, 16);
	return res;
}

// rC004.D -> 0xC004 (49156)
u16 ModbusSlave::BitRegAddrToU16(char* RegAddr) {
	return 0;
}

// rC004.H -> 0xC004 (49156)
// rC004.L -> 0xC004 (49156)
u16 ModbusSlave::ByteRegAddrToU16(char* RegAddr) {
	return 0;
}
*/