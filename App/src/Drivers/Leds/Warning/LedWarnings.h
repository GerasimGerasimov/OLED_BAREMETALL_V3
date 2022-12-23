//служба сообщений
#ifndef LED_WARNINGS_H
#define LED_WARNINGS_H

#include "stm32f4xx.h"

class LedWarnings {
	public:
		static void init();
		static void setState(u16 state);
};

#endif