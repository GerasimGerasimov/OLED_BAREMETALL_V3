#ifndef HANDLE_SUBSCRIBERS_H
#define HANDLE_SUBSCRIBERS_H

#include <string>
#include <list>
#include <map>
#include <functional>
#include "Slot.h"
#include "SlotHandlerType.h"

typedef struct {
	std::string device;
	std::string section;
} THandlerSearchStruct;

typedef struct TSubscriber {
	int id;
	std::function<void(TSlotHandlerArsg)> handler;
};

class HandlerSubscribers {
public:
	static int set(std::string source, std::function<void(TSlotHandlerArsg)> handler);
	static void remove(std::string source, int& ID);
	static void send(Slot* slot);

private:
	static std::map<std::string, std::map<std::string, std::list<TSubscriber>>> Handlers;
};

#endif