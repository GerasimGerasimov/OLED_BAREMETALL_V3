//служба сообщений
#ifndef WARNINGS_H
#define WARNINGS_H

#include "stm32f4xx.h"
#include "HandleSubscribers.h"
#include <StateControllers.h>

class Warnings {
	public:
		static void init(void);
		static bool isTagInWarning(TTrackedBit& element);
		static std::map < std::string, TTrackedBit > Tags;
	private:
		static void uptate(const std::string PosMem, TSlotHandlerArsg& args);
		static bool checkState(void);
		static void SlotU1RAMUpdate(TSlotHandlerArsg args);
		static bool State;
};

#endif