#include "IniResources.h"
#include "parser.h"
#include "IniSlotsProps.h"
#include "signal.h"
#include <vars.h>

const static std::vector<std::string> SectioNameOrder = { "vars", "RAM", "FLASH", "CD"};

const static std::map<std::string, std::string> SectionNameMap = { {"vars","[vars]"}, {"RAM","[RAM]"}, {"FLASH","[FLASH]"},{"CD","[CD]"} };

std::map<std::string, std::map<std::string, std::map<std::string,ISignal*>>> IniResources::Sources = {};
/*TODO обеспечить такую структуру
U1
 |-vars
 |   |-S1=
 |
 |-RAM
 |   |-p0=
 |
 |-FLASH
 |-CD
*/

void IniResources::init(void) {
	readSources();
}

/*TODO Проверить: эта функция ни где не используется?*/
std::string IniResources::getComment(const TValueSearchStruct& srch) {
	if (Sources.count(srch.device)) {
		std::map<std::string, std::map<std::string, ISignal*>> devmap = Sources.at(srch.device);
		if (devmap.count(srch.section)) {
			std::map<std::string, ISignal*> signals = devmap.at(srch.section);
			if (signals.count(srch.name)) {
				ISignal* s = signals.at(srch.name);
			}
		}
	}
	/*
	ISignal* s = */
	return std::string();
}

/*выдать указатель на сигнал по его тегу*/
//tag = "U1/RAM/Iexc/"
//узнать какой DEVx является описанием для Ux цстройства
//и перекодировать U1/RAM/Iexc/ в DEV1/RAM/Iexc/
TValueSearchStruct IniResources::TagToValueSearchStruct(const std::string& tag) {
	std::vector<std::string> v = IniParser::getListOfDelimitedString(
		(const char)'/',
			(char*)tag.c_str(),
				tag.size());
	TValueSearchStruct res;
	if (v.size() == 3) {
		/*TODO тут явно плохое место! два взаимозависимых модуля
		эти модули нельзя переставить местами при инициализации!
		сначала инициализация IniResources
		потом я могу инициализировать IniSlotsProps
		а тут я использую данные от обоих модулей, связываю их
		Эту функцию надо выделить как статический класс*/
		std::string dev = IniSlotsProps::getSourceOfDev(v[0]);
		if (dev != "") {
			return { dev, v[1], v[2] };
		}
	}
	else
		return { "", "", "" };
}

std::string IniResources::getDevicePositionByTag(const std::string& tag) {
	std::vector<std::string> v = IniParser::getListOfDelimitedString(
		(const char)'/',
		(char*)tag.c_str(),
		tag.size());
	return (v.size() == 3)
		? v[0]
		: "";
}

u16 IniResources::getDevNetWorkAddrByTag(const std::string& tag) {
	std::vector<std::string> v = IniParser::getListOfDelimitedString(
		(const char)'/',
		(char*)tag.c_str(),
		tag.size());
	TValueSearchStruct res;
	if (v.size() == 3) {
		/*TODO тут явно плохое место! два взаимозависимых модуля
		эти модули нельзя переставить местами при инициализации!
		сначала инициализация IniResources
		потом я могу инициализировать IniSlotsProps
		а тут я использую данные от обоих модулей, связываю их
		Эту функцию надо выделить как статический класс*/
		u16 addr = IniSlotsProps::getDevNetWorkAddr(v[0]);
		return addr;
	}
	else
		return 0;;
}

/*TODO найти ключ в секции vars заданного DEV и выдать его значние*/
std::string IniResources::getScaleValueByKey(const std::string& key, const std::string& dev) {
	if (Sources.count(dev)) {
		std::map<std::string, std::map<std::string, ISignal*>> devmap = Sources.at(dev);
		if (devmap.count("vars")) {
			std::map<std::string, ISignal*> vars = devmap.at("vars");
			if (vars.count(key)) {
				TScale* s = (TScale*)vars.at(key);
				return s->getValue();
			}
		}
	}
	return "1.0";
}

ISignal* IniResources::getSignalByTag(const std::string& tag) {
	TValueSearchStruct srch = TagToValueSearchStruct(tag);
	if (Sources.count(srch.device)) {
		std::map<std::string, std::map<std::string, ISignal*>> devmap = Sources.at(srch.device);
		if (devmap.count(srch.section)) {
			std::map<std::string, ISignal*> signals = devmap.at(srch.section);
			if (signals.count(srch.name)) {
				ISignal* s = signals.at(srch.name);
				return s;
			}
		}
	}
	TSignalPropsPointers defaultprops = { NULL, NULL, NULL, NULL }; 
	return new ISignal(defaultprops);
}

bool IniResources::readSources(void)
{
	TItemLimits itemLimits = TInternalResources::getItemLimitsByName((char*)"SOURCES");
	if (itemLimits.RootOffset) {
		IniParser::setRoot(itemLimits.RootOffset, itemLimits.Size);
		std::vector<std::string> sources = IniParser::getListOfDelimitedString('/', itemLimits.RootOffset, itemLimits.Size);
		for (auto& src : sources) {
			itemLimits = TInternalResources::getItemLimitsByName((char*)src.c_str());
			if (itemLimits.RootOffset) {
				IniParser::setRoot(itemLimits.RootOffset, itemLimits.Size);
				for (auto& section : SectioNameOrder) {
					std::string value = SectionNameMap.at(section);
					if (IniParser::setSectionToRead((char*)value.c_str())) {
						//std::vector<ISignal*> params = {};
						TSectionReadResult readResult = { NULL, 0 };
						while ((readResult = IniParser::getNextTagString()), readResult.result != 0) {
							 ISignal* s = (section == "vars")
								? TIniString::getScale(section, readResult.tag, readResult.result)
								: TIniString::getSignal(src, section, readResult.tag, readResult.result);
							 if (s) {
								 std::string name = s->getName();
								 Sources[src][section][name] = s;
							 }
						}
					}
				}
			}
		}
	}
	return false;
}