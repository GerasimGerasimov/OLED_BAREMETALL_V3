#include "HandleSubscribers.h"
#include <parser.h>

std::map<std::string, std::map<std::string, std::vector<std::function<void(TSlotHandlerArsg)>>>> HandlerSubscribers::Handlers = {};

void HandlerSubscribers::set(std::string source, std::function<void(TSlotHandlerArsg)> handler) {
	/*source это строка типа "U1/RAM/" тут её распарсю и добавляю в Handlers
	  чтобы получилось
	  U1--
	     |-RAM--
		       |-function*/
	/*TODO И вернуть bool чтои что бы знать что добавилось или нет*/
	std::vector<std::string> v = IniParser::getListOfDelimitedString(
		(const char)'/',
		(char*)source.c_str(),
		source.size());
	std::string dev = v[0];
	std::string section = v[1];
	/*надо дабавлять не один а несколько обработчиков*/
	Handlers[dev][section].push_back(handler);
}

void HandlerSubscribers::remove(std::string source) {
	/*TODO удалить обработчик "U1/RAM/" из Handlers*/
}

void HandlerSubscribers::send(Slot& slot /*TODO аргумент "U1/RAM/"*/) {
	//найти все обработчики соответсвующие аргументу "U1/RAM/" и последовательно вызвать их
	if (Handlers.count(slot.Device)) {
		std::map<std::string, std::vector<std::function<void(TSlotHandlerArsg)>>> sections = Handlers.at(slot.Device);
		if (sections.count(slot.Section)) {
			std::vector<std::function<void(TSlotHandlerArsg)>> handlers = sections.at(slot.Section);
				for (auto& handler : handlers) {
					TSlotHandlerArsg args = { &slot.InputBuf[0], slot.InputBufValidBytes, slot.StartAddrOffset, slot.LastAddrOffset };
					handler(args);
				}
		}
	}
}