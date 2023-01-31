#include "TBit.h"
#include "parser.h"
#include "ParametersUtils.h"
#include "bastypes.h"
#include <array>
//Special case Two
//p13321 = P / C / TBit / xF042 / mask / r2021.1 / 1 / 1 / const / вообще нет MSU
/*TODO Addr
typedef struct {
	u16 Addr;
	u16 Option;
} TSpecialSignalAddr;
В Addr я запишу собсвенно смещение
а в Option - запишу маску, которую и буду накладывать чтобы получить значение
r2021.1 в таком виде получаю адрес, из него надо выделить собственно адрес и новер бита на основании которого сформировать маску.
*/
TBit::TBit(TSignalPropsPointers props)
	: TParameter(props) {
	strAddr = IniParser::getElementPtrByNumber(2, '/', props.pOptional);
	Addr = ParametersUtils::getSpecialAddrForBit(strAddr);	
}

TBit::~TBit(){
}

std::string TBit::getValue(const TSlotHandlerArsg& args, const char* format) {
	std::string res = validation(args);
	return (res != "")
		? res
		: value(args);
}

const std::string TBit::value(const TSlotHandlerArsg& args) {
	bool input = getRawValue(args);
	return (input)?"1":"0";
}

const std::string TBit::getValueHex(std::string& src) {
	float f = std::stof(src);
	std::string res = (f != 0) ? "1" : "0";
	return res;
}

const std::string TBit::getRegHexAddr() {
	std::string res(strAddr + 1, 6);
	return res;
}

bool TBit::getRawValue(const TSlotHandlerArsg& args) {
	s16 offset = Addr.Addr - args.StartAddrOffset;
	u8* p = args.InputBuf + offset;//получил указатель на данные
	bauint raw;//получил два байта данных
	raw.b[0] = (*p++);
	raw.b[1] = (*p);
	bool res = (bool)(raw.i & Addr.Option);
	return res;
}

const std::string TBit::validation(const TSlotHandlerArsg& args) {
	if (args.InputBufValidBytes == 0) return "*";
	if (ParametersUtils::isAddrInvalid(Addr.Addr)) return "err.addr";
	if ((Addr.Addr < args.StartAddrOffset) || (Addr.Addr > args.LastAddrOffset)) return "out.addr";
	return "";
}

const std::string TBit::getWriteCmdType() {
	return "16";
}

static const std::string SignalType = "TBit";
const std::string& TBit::getSignalType() {
	return SignalType;
}

static const std::array<u16,16> Mask2BitNumber = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768};
TInternalMemAddress TBit::getInternalMemAddr() {
	s16 offset = Addr.Addr;
	s16 bitNumber = -1;
	u16 index = 0;
	for (auto& e : Mask2BitNumber) {
		if (e == Addr.Option) {
			bitNumber = index;
			break;
		}
		index++;
	}
	offset += (bitNumber < 7) ? 0 : 1;
	bitNumber -= (bitNumber < 7) ? 0 : 8;
	return { offset, 1, bitNumber };
}