#include "TU16BIT.h"
#include "bastypes.h"

TU16BIT::TU16BIT(TSignalPropsPointers props)
	: TGeneralCaseSignal(props) {
}

TU16BIT::~TU16BIT(){
}

TGenaralCaseRawReturn TU16BIT::getRawValue(const TSlotHandlerArsg& args) {
	s16 offset = Addr - args.StartAddrOffset;
	u8* p = args.InputBuf + offset;//получил указатель на данные
	bauint raw;//получил два байта данных
	raw.b[0] = (*p++);
	raw.b[1] = (*p);
	TGenaralCaseRawReturn res;
		res.type = TGeneralCaseReturnType::U;
		res.raw.i = raw.i;
	return res;
}

//Результат в виде строки я принял из-за того, что эта струкрута содержит и значения и длину
//напр эта функция вернёт строку "NNNN" длиной 4 символа
const std::string TU16BIT::getValueHex(std::string& src) {
	u16 value = string2raw(src);
	char s[8];
	//GIST "%.4X" преобразование числа в hex с заданным кол-вом значащих нулей
	sprintf(s, "%.4X", value);
	std::string res(s);
	return res;
}

u16 TU16BIT::string2raw(std::string& src) {
	float f = std::stof(src);
	f /= Scale;
	return (u16)f;
}

TInternalMemAddress TU16BIT::getInternalMemAddr() {
	s16 offset = Addr;
	return { offset,2,-1 };
}