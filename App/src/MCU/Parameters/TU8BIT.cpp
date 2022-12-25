#include "TU8BIT.h"
#include "parser.h"
#include "ParametersUtils.h"
#include "bastypes.h"
#include "scaleutils.h"
#include "Utils.h"

TU8BIT::TU8BIT(TSignalPropsPointers props)
	: TParameter(props)
	, MSU (IniParser::getElementPtrByNumber(2, '/', props.pOptional)) {
	strAddr = IniParser::getElementPtrByNumber(1, '/', props.pOptional);
	Addr = ParametersUtils::getSpecialAddrForByte(strAddr);
	Scale = ScaleUtils::getScaleFromProps(props.dev, props.pOptional);
}

TU8BIT::~TU8BIT(){
}

std::string TU8BIT::getMSU()
{
	return (MSU)
		? IniParser::getElement('/', MSU)
		: "";
}

void TU8BIT::setValue() {}

std::string TU8BIT::getValue(const TSlotHandlerArsg& args, const char* format) {
	std::string res = validation(args);
	return (res != "")
		? res
		: value(args, format);
}

const std::string TU8BIT::value(const TSlotHandlerArsg& args, const char* format) {
	u8 input = getRawValue(args);
	float res = input * Scale;
	return Utils::getValueAsFormatStr(res, Utils::getFormat(res));
}

u8 TU8BIT::getRawValue(const TSlotHandlerArsg& args) {
	s16 offset = Addr.Addr - args.StartAddrOffset;
	u8* p = args.InputBuf + offset;//получил указатель на данные
	bauint raw;//получил два байта данных
	raw.b[0] = (*p++);
	raw.b[1] = (*p);
	u8 res = (Addr.Option == 1)? raw.b[1] : raw.b[0];
	return res;
}

const std::string TU8BIT::validation(const TSlotHandlerArsg& args) {
	if (args.InputBufValidBytes == 0) return "**.*";
	if (ParametersUtils::isAddrInvalid(Addr.Addr)) return "err.addr";
	if ((Addr.Addr < args.StartAddrOffset) || (Addr.Addr > args.LastAddrOffset)) return "out.addr";
	return "";
}