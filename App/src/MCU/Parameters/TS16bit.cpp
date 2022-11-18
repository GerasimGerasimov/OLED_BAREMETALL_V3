#include "TS16BIT.h"
#include "bastypes.h"

TS16BIT::TS16BIT(TSignalPropsPointers props)
	: TGeneralCaseSignal (props) {
}

TS16BIT::~TS16BIT(){
}

TGenaralCaseRawReturn TS16BIT::getRawValue(const TSlotHandlerArsg& args) {
	s16 offset = Addr - args.StartAddrOffset;
	u8* p = args.InputBuf + offset;//получил указатель на данные
	bauint raw;//получил два байта данных
	raw.b[0] = (*p++);
	raw.b[1] = (*p);
	TGenaralCaseRawReturn res;
	res.type = TGeneralCaseReturnType::S;
	res.raw.s = raw.s;
	return res;
}