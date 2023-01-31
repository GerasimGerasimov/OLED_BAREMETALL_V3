#include "TPrmList.h"
#include "parser.h"
#include "ParametersUtils.h"
#include "bastypes.h"
#include "scaleutils.h"
#include "Utils.h"

TPrmList::TPrmList(TSignalPropsPointers props)
	: TParameter(props)
	, MSU (IniParser::getElementPtrByNumber(2, '/', props.pOptional)) {
	strAddr = IniParser::getElementPtrByNumber(1, '/', props.pOptional);
	Addr = ParametersUtils::getSpecialAddrForByte(strAddr);
	Scale = ScaleUtils::getScaleFromProps(props.dev, props.pOptional);
}

TPrmList::~TPrmList(){
}

std::string TPrmList::getMSU()
{
	return (MSU)
		? IniParser::getElement('/', MSU)
		: "";
}

void TPrmList::setValue() {}

//p501=RS485_1_BPS/Baudrate/TPrmList/xF001/r2000.H/BPS//x01#9600/x02#19200/x03#57600/x04#115200/x04/

std::string TPrmList::getValue(const TSlotHandlerArsg& args, const char* format) {
	std::string res = validation(args);
	return (res != "")
		? res
		: value(args, format);
}

std::vector<std::string> TPrmList::getList(void) {
	char* options = optional;//приходится сохранять указатели так как метод ниже меняет его
	char* plist = IniParser::getElementPtrByNumber(4, '/', options);
	char* listAddr = plist;//приходится сохранять указатели так как метод ниже меняет его
	int size = IniParser::getStringLenght(&listAddr);
	std::vector<std::string> list = IniParser::getListOfDelimitedString('/', plist, size);
	std::vector<std::string> res = {};
	for (auto& e : list) {
		int index = e.find('#');
		if (index == std::string::npos) continue;
		std::string s = e.substr(index+1);
		res.push_back(s);
	}
	return res;
}

std::vector<std::string> TPrmList::getList(const std::string& val, s16& ValueIndexInList) {
	std::vector<std::string> list = getList();
	std::vector<std::string>::iterator it = std::find(list.begin(), list.end(), val);
	if (it != list.end()) {
		ValueIndexInList = std::distance(list.begin(), it);
	}
	else {
		ValueIndexInList = -1;
	}
	return list;
}

const std::string TPrmList::value(const TSlotHandlerArsg& args, const char* format) {
	u8 input = getRawValue(args);
	//найти 7-й слэш
	// [7+0]            [7+1]                [7+N]
	// /index0#data0#ai0/index1#data1#ai1/.../indexN#dataN#aiN/Base/
	// Вase отличается тем, что в нём нет символа '#'
	char* options = optional;//приходится сохранять указатели так как метод ниже меняет его
	char* plist = IniParser::getElementPtrByNumber(4, '/', options);
	char* listAddr = plist;//приходится сохранять указатели так как метод ниже меняет его
	int size = IniParser::getStringLenght(&listAddr);
	std::vector<std::string> list = IniParser::getListOfDelimitedString('/', plist, size);
	//Список получил
	//обрататываю строки вида x01#9600, надо выделить число до # (вдс x01 сравнить его с input)
	//и если совпало, то выделяю строку после #, это и будет результат
	for (auto& e : list) {
		int index = e.find('#');
		if (index == std::string::npos) continue;
		u8 firstentry = (e[0] == 'x') ? 1 : 0;
		e[index] = 0;//конец строки
		int value = std::stoi(&e.c_str()[firstentry], 0, 16);
		if (value == input) {
			std::string res = std::string(e, index + 1);
			return res;
		}
	}
	return "?";
}

std::string TPrmList::getKeyByValue(const std::string& val) {
	char* options = optional;//приходится сохранять указатели так как метод ниже меняет его
	char* plist = IniParser::getElementPtrByNumber(4, '/', options);
	char* listAddr = plist;//приходится сохранять указатели так как метод ниже меняет его
	int size = IniParser::getStringLenght(&listAddr);
	std::vector<std::string> list = IniParser::getListOfDelimitedString('/', plist, size);
	for (auto& e : list) {
		int index = e.find('#');
		if (index == std::string::npos) continue;
		std::string s = e.substr(index + 1);
		if (s == val) {
			std::string res = e.substr(0, index);
			return res;
		}
	}
	return "";
}

const std::string TPrmList::getValueHex(std::string& src) {
	//u16 value = string2raw(src);
	//char s[8];
	//GIST "%.4X" преобразование числа в hex с заданным кол-вом значащих нулей
	//sprintf(s, "%.4X", value);
	//std::string res(s);
	int xpos = (src[0] == 'x') ? 1 : 0;
	std::string res = src.substr(xpos);
	return res;
}

const std::string TPrmList::getRegHexAddr() {
	std::string res(strAddr + 1, 6);
	return res;
}


u8 TPrmList::getRawValue(const TSlotHandlerArsg& args) {
	s16 offset = Addr.Addr - args.StartAddrOffset;
	u8* p = args.InputBuf + offset;//получил указатель на данные
	bauint raw;//получил два байта данных
	raw.b[0] = (*p++);
	raw.b[1] = (*p);
	u8 res = (Addr.Option == 1)? raw.b[1] : raw.b[0];
	return res;
}

const std::string TPrmList::validation(const TSlotHandlerArsg& args) {
	if (args.InputBufValidBytes == 0) return "?";
	if (ParametersUtils::isAddrInvalid(Addr.Addr)) return "err.addr";
	if ((Addr.Addr < args.StartAddrOffset) || (Addr.Addr > args.LastAddrOffset)) return "out.addr";
	return "";
}

const std::string TPrmList::getWriteCmdType() {
	return "16";
}

static const std::string SignalType = "TPrmList";
const std::string& TPrmList::getSignalType() {
	return SignalType;
}

TInternalMemAddress TPrmList::getInternalMemAddr() {
	s16 offset = Addr.Addr;
	offset += Addr.Option;//0-L, 1-H byte
	return { offset,1,-1};
}