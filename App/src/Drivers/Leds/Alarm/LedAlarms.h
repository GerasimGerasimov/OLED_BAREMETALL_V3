//служба сообщений
#ifndef LED_ALARMS_H
#define LED_ALARMS_H

#include "stm32f4xx.h"

class LedAlarms {
	public:
		static void init();
		static void setState(u16 state);
};

#endif