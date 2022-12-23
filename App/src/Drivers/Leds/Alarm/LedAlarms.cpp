#include "LedAlarms.h"

void LedAlarms::init() {
	setState(1);
}

void LedAlarms::setState(u16 state) {
	switch (state) {
		case 0:
			//FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
			break;
		case 1:
			//FillRect(hdc, &rect, RED_BRUSH);
			break;
	};
}
