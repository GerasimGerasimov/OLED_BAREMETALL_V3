#ifndef COMMON_SLOT_HANDLER_H
#define COMMON_SLOT_HANDLER_H
#include "IniSlotsProps.h"
#include "Slot.h"

class CommonSlotHandler {
public:
	static void parseRespond(Slot& slot, u8* reply);//u8* reply ещё не скопирован в Слот! это источник данных,
	                                                //так как требуется обработка, например swap полученных данных для 0х03 команды чтения
};

#endif