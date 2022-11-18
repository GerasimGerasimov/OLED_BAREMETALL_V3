#include "generalcase.h"
#include "parser.h"
#include "ParametersUtils.h"
#include "scaleutils.h"
#include "Utils.h"

TGeneralCaseSignal::TGeneralCaseSignal(TSignalPropsPointers props)
	: TParameter(props)
	, MSU(IniParser::getElementPtrByNumber(2, '/', props.pOptional)) {
	strAddr = IniParser::getElementPtrByNumber(1, '/', props.pOptional);
	Addr = ParametersUtils::getByteOffsetFromSlahedAddrStr(strAddr);
	Scale = ScaleUtils::getScaleFromProps(props.dev, props.pOptional);
}

TGeneralCaseSignal::~TGeneralCaseSignal(){
}

/*
Где находится MSU

//Special case One
p19802=P/C/TPrmList/xF0C9/r2064.H/msu//x03#9600/x04#19200/x05#57600/x06#115200/const/

//General case
//общий случай для MSU.. но TODO сначала надо распарсить option там содержится msu 
                   |options
				   |0     1       2
p65301=P/C/TByte   /xE06A/rC035.L/msu/scale/bytes//const/
p17100=P/C/TInteger/xF090/r2048  /msu/scale/bytes//const/
p60200=P/C/TWORD   /xE004/rC002  /msu/scale/bytes//const/
p60000=P/C/TDWORD  /xE000/rC000  /msu/scale/bytes//const/
p62800=P/C/TFloat  /xE038/rC01C  /msu/scale/bytes//const/
*/

std::string TGeneralCaseSignal::getMSU()
{
	return (MSU)
		? IniParser::getElement('/', MSU)
		: "";
}

void TGeneralCaseSignal::setValue() {}

std::string TGeneralCaseSignal::getValue(const TSlotHandlerArsg& args, const char* format) {
	std::string res = validation(args);
	return (res != "")
		? res
		: value(args, format);
}

typedef float (*TFuncRawToFloat) (TGenaralCaseRawReturn& input);

static inline float getFloatFromU(TGenaralCaseRawReturn& input) {
	float res = (float)input.raw.i;
	return res;
}

static inline float getFloatFromS(TGenaralCaseRawReturn& input) {
	float res = (float)input.raw.s;
	return res;
}

static inline float getFloatFromF(TGenaralCaseRawReturn& input) {
	float res = input.raw.f;
	return res;
}

static const TFuncRawToFloat FuncRawToFloat[] = {
	getFloatFromU,
	getFloatFromS,
	getFloatFromF
};

static inline float RawToFloat(TGenaralCaseRawReturn& input) {
	float res = FuncRawToFloat[(u8)input.type](input);
	return res;
}

std::string TGeneralCaseSignal::value(const TSlotHandlerArsg& args, const char* format) {
	TGenaralCaseRawReturn input = getRawValue(args);
	float res =  RawToFloat(input) * Scale;
	return Utils::getValueAsFormatStr(res, format);
}

TGenaralCaseRawReturn TGeneralCaseSignal::getRawValue(const TSlotHandlerArsg& args) {
	return { TGeneralCaseReturnType::U, 0};
}

std::string TGeneralCaseSignal::validation(const TSlotHandlerArsg& args) {
	if (args.InputBufValidBytes == 0) return "***.**";
	if (ParametersUtils::isAddrInvalid(Addr)) return "err.addr";
	if ((Addr < args.StartAddrOffset) || (Addr > args.LastAddrOffset)) return "out.addr";
	return "";
}