#ifndef ALARMS_H
#define ALARMS_H

#include "stm32f4xx.h"
#include "HandleSubscribers.h"
#include <StateControllers.h>

class Alarms {
	public:
		static void init(void);
		static bool isTagAlarmed(TTrackedBit& element);
		static bool isAlarmOnce(void);
		static std::map < std::string, TTrackedBit > Tags;
	private:
		static void uptate(const std::string PosMem, TSlotHandlerArsg& args);
		static bool checkState(void);
		static void SlotU1RAMUpdate(TSlotHandlerArsg args);
		static bool State;
		static u8 UppedFlags;
		static u8 PrevUppedFlags;
};

#endif