#include "scaleutils.h"
#include "vars.h"
#include "Utils.h"
#include "parser.h"
#include "IniResources.h"

float ScaleUtils::getScaleFromProps(const char* dev, const char* optional) {
	char* pScale = IniParser::getElementPtrByNumber(3, '/', (char*) optional);
	std::string s = IniParser::getElement('/', pScale);
	Utils::comma_to_dot((char*)s.c_str());
	float res = 1.0f;
	try {
		res = std::stof(s);
	}
	catch (...) {
		//попал сюда, потому что строка не преобразовалась в float
		//возможно это имя ключа из секции [vars] но тогда надо знать к какому DEV это относится. 
		std::string val = getVarStrValueByKey(s, dev);
		Utils::comma_to_dot((char*)val.c_str());
		try {
			res = std::stof(val);
		}
		catch (...) {
			//TODO как сюда попал?!
		}

	};
	/*TODO если строка без исключения трансформируется во float то на этом останавливаюсь*/
	/*TODO если возникло исключение, то пробую найти строку в vars
		если нахожу, то беру её значение
			если значение переводится без исключения в float на этом останавливаюсь
			если возникло исключение, то возможно это список!, надо его парсить и из него извлекать значение
				если в списке нет соответсвующего значения, то возвражаю "1"
			*/
			/*TODO если ни каким образом не получилось достать значение коэффициента, то вернуть "1"*/
	return res;
}

std::string ScaleUtils::getVarStrValueByKey(std::string& key, const char* dev) {
	std::string _dev(dev);/*GIST преобразование char* в std::string*/
	std::string sval = IniResources::getScaleValueByKey(key, _dev);
	return sval;
}