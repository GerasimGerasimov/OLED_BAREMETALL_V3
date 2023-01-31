#ifndef MAIN_MCU_APP_H
#define MAIN_MCU_APP_H

#include "stm32f4xx.h"

class App {
	public:
        static bool init(void);
        static void run(void);
        static void error(void);
};
#endif
