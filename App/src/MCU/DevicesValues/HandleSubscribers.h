#ifndef HANDLE_SUBSCRIBERS_H
#define HANDLE_SUBSCRIBERS_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include "Slot.h"
#include "SlotHandlerType.h"

typedef struct {
	std::string device;
	std::string section;
} THandlerSearchStruct;

class HandlerSubscribers {
public:
	static void set(std::string source, std::function<void(TSlotHandlerArsg)> handler);
	static void remove(std::string source);
	static void send(Slot& slot);

private:
	static std::map<std::string, std::map<std::string, std::vector<std::function<void(TSlotHandlerArsg)>>>> Handlers;
};

#endif