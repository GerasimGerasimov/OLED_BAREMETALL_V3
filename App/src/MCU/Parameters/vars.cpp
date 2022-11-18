#include "vars.h"
#include "parser.h"
#include "IniResources.h"

TScale::TScale(char* source, int scrLen) : ISignal(source, scrLen) {
}

TScale::TScale(TScaleProps props)
	: ISignal({NULL, props.pKey, NULL, NULL, props.pValue})
	, ValueSize(props.ValueSize) {
}

TScale::~TScale(){
}

std::string TScale::getName() {
	std::string s = (Name)
		? IniParser::getElement('=', Name)
		: "";
	return s;
}

std::string TScale::getValue() {
	/*тут надо задействовать ValueSize так как строка может быть как простым значением
	так и массивом строк разделённых слэшами... отнесусь я к ней как к массиву
	и если у массива будет только один элемент, значит в нём есть только значение*/
	//std::string opt = "x00#15-25A#0,16625/x01#10-15A#0,100377/x02#5-10A#0,041953/x04#1,5-5A#0,015385/x08#0,5-1,5A#0,003715/x0F#0,25-1,5A#0,004576/x08";
	//std::string opt = "x00#15-25A#0,16625/x01#10-15A#0,100377/x02#5-10A#0,041953/x04#1,5-5A#0,015385/x08#0,5-1,5A#0,003715/x0F#0,25-1,5A#0,004576/x08/";
	//std::string opt = "0,16625";
	//ValueSize = opt.size();
	//std::vector<std::string> values = IniParser::getListOfDelimitedStrInclude('/', (char*)opt.c_str(), ValueSize);
	std::vector<std::string> values = IniParser::getListOfDelimitedStrInclude('/', optional, ValueSize);
	return (values.size() == 1)
		? values[0]
		: (values.size() == 0)
			? "1.0"
			: getValueFromList(values);
}

std::string TScale::getValueFromList(std::vector<std::string> values) {
	/*TODO список вариантов, с номером выбранного варианта в конце*/
	return "1.0";
}