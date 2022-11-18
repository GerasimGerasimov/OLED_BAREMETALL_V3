#include "TFloat.h"
#include "bastypes.h"

TFloat::TFloat(TSignalPropsPointers props)
	: TGeneralCaseSignal(props) {
	
}

TFloat::~TFloat(){
}

TGenaralCaseRawReturn TFloat::getRawValue(const TSlotHandlerArsg& args) {
	s16 offset = Addr - args.StartAddrOffset;
	u8* p = args.InputBuf + offset;//получил указатель на данные
	baulong  raw;//получил два байта данных
	raw.b[0] = (*p++);
	raw.b[1] = (*p++);
	raw.b[2] = (*p++);
	raw.b[3] = (*p);
	TGenaralCaseRawReturn res;
	res.type = TGeneralCaseReturnType::F;
	res.raw.f = raw.f;
	return res;
}
