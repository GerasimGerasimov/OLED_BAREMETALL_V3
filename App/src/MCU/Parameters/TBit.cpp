#include "TBit.h"
#include "parser.h"
#include "ParametersUtils.h"
#include "bastypes.h"
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