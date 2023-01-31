#include "AppModbusSlave.h"
#include <parameters.h>
#include <IniResources.h>
#include <DevicePollManager.h>

static u16 getTimeOutBySection(const std::string& section) {
    return (section == "RAM")
        ? 500
        : 2000;
}

bool ModbusSlave::setValue(std::string& tag, std::string& value, TSlotDataHandler callback) {
    TParameter* p = (TParameter*)IniResources::getSignalByTag(tag);
    std::string ValueHex = p->getValueHex(value);//получил значение в хексах и сразу длину (так как строка)
    std::string RegHexAddr = p->getRegHexAddr();//получил номер регистра в хексах
    const std::string Cmd = p->getWriteCmdType();//тип команды записи 10 или 16
    /*по Tag надо узнаю на какой Адрес отправлять
        "DEVICES":"U1/", есть список устройств, надо сопостоавить из тега U1
    "U1":"DEV1/COM1/1 а из U1 найти адрес (он за COM1/)*/
    const TValueSearchStruct Info = IniResources::spliceTagInfo(tag);
    const u16 DevAddr = IniResources::getDevNetWorkAddrByDevPos(Info.device);  
    const std::string DevAddrHex = ModbusSlave::NetWorkAddrToHex(DevAddr);
    const std::string Section = "CmdWrite";
    Slot* slot = DevicePollManager::getSlotByDevPosAndSection(Info.device, Section);
    slot->TimeOut = getTimeOutBySection(Info.section);
    slot->cmdLen = ModbusSlave::CreateWriteCmd(slot->OutBuf, { Cmd, DevAddrHex, RegHexAddr, ValueHex });
    return (slot->cmdLen)
        ? (slot->onData = callback,
            slot->Flags &= ~(u16)SlotStateFlags::SKIP_SLOT, //пометить слот на выполнение
                          //| (u16)SlotStateFlags::CRC_ERR //сброс ошибок... хотя ошибки будут анализироваться позже
                          //| (u16)SlotStateFlags::TIMEOUT_ERR
                          //| (u16)SlotStateFlags::DATA_VALID );
            true)
        : false;
}

const std::map < std::string, std::function <u8(u8*, TWriteCmdSrc&) >> ModbusSlave::WriteCmdVariants = {
    {"10", [](u8* a, TWriteCmdSrc& props) {return get0x10WriteRegCmd(a, props); }},
    {"16", [](u8* a, TWriteCmdSrc& props) {return get0x16MaskWriteRegisterCmd(a, props); }},
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
        ? (
            a[7] = (value & 0x0000FF00) >> 8,
            a[8] = (value & 0x000000FF) >> 0
            )
        : (
            a[7] = (value & 0x0000FF00) >> 8,
            a[8] = (value & 0x000000FF) >> 0,
            a[9] = (value & 0xFF000000) >> 24,
            a[10] = (value & 0x00FF0000) >> 16
            );
    //контрольная сумма 
    u8 cmdLen = (regscnt == 1) ? 11 : 13;
    FrameEndCrc16(a, cmdLen);
    return cmdLen;
}

/*  Mask Write Register
    (0x16) — запись в один регистр хранения с использованием маски «И» и маски «ИЛИ» (Mask Write Register).
    http://microsin.net/programming/arm/modbus-application-protocol-specification.html
    https://forum.segnetics.com/showthread.php?s=4124ffd14c15603d39147d08335ce1cf&t=4654&page=2
    https://www.manualslib.com/manual/1298275/Mitsubishi-Fx3u-Series.html?page=80
    https://control.com/forums/threads/modbus-fc22-0x16-mask-write-register.37596/

запрос 0x16 на выходе
AD, C, №R(h, l), AND(h, l), OR(h, l), CRC(h, l)
│   │  │          │          │        └─(word)crc16    
│   │  │          │          └─(word)Маска OR
│   │  │          └─(word)Маска AND
│   │  └─(word)Адрес регистра
│   └────(byte)Команда(16h)
└──────(byte)Адрес устройства
Результат = (текущее содержимое & маскаAND) | (маскаOR & (^маскаAND))
*/

/*Адрес поступает сюда в виде:
для байт (старшего/младшего )rrrr.H, rrrr.L,
для бит rrrr.N, где N-номер бита
Поэтому, для БАЙТ и БИТ надо сделать немного разную обработку по вычислению маски AND и OR
Для БАЙТ:
    H, AND=0x00FF (не трогаю младшие 8 бит, старшие 8 обнуляю)
       OR = (value << 8) & 0xFF00
    L, AND=0xFF00 (не трогаю станшие 8 бит, младшие 8 обнуляю)
       OR = value & 0x00FF

Для БИТ:
 N - номер бита известен
    AND = ~(value<<N) -чищу данные инверсным значением
    OR = (value<<N) & !AND
*/

u8 ModbusSlave::get0x16MaskWriteRegisterCmd(u8* a, TWriteCmdSrc& Src) {
    a[0] = (u8)std::stoi(Src.DevAddr);
    a[1] = 0x16;

    int index = Src.RegAddr.find('.');
    if (index == std::string::npos) {
        return 0;//это не адрес БАЙТа или БИТа (rrrr.H/L/N)
    }
    //начальный адрес регистра
    std::string Addr = Src.RegAddr.substr(0, 4);
    u16 addr = (u16)std::stoul(Addr, nullptr, 16);
    a[2] = (u8)(addr >> 8) & 0x00FF;
    a[3] = (u8)(addr & 0x00FF);

    std::string BT = Src.RegAddr.substr(5);
    u16 value = std::stol(Src.Value, nullptr, 16);
    u16 AND_MASK = 0;
    u16 OR_MASK = 0;
    if (BT == "H") {
        AND_MASK = 0x00FF;
        OR_MASK = (value << 8) & 0xFF00;
    } else if (BT == "L") {
        AND_MASK = 0xFF00;
        OR_MASK = (value) & 0x00FF;
    } else {//это БИТы 0123456789ABCDEF
        /*TODO ПРОВЕРИТЬ!*/
        bool bitValue = (bool)(value != 0);
        u16 bitnum = std::stol(BT, nullptr, 16);
        u16 BIT_MASK = (1 << bitnum);
        AND_MASK = ~BIT_MASK;
        OR_MASK = BIT_MASK;
    }
    a[4] = (u8)(AND_MASK >> 8) & 0x00FF;
    a[5] = (u8)(AND_MASK & 0x00FF);
    a[6] = (u8)(OR_MASK >> 8) & 0x00FF;
    a[7] = (u8)(OR_MASK & 0x00FF);
    u8 cmdLen = 10;
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