#include "LedWarnings.h"

void LedWarnings::init() {
	setState(1);
}

void LedWarnings::setState(u16 state) {
	switch (state) {
		case 0:
			//FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
			break;
		case 1:
			//FillRect(hdc, &rect, YELLOW_BRUSH);
			break;
	};
}
