#include "DevicesValuesHandler.h"
#include "HandleSubscribers.h"

void DevicesValuesHandler::setValues(Slot& slot) {
	HandlerSubscribers::send(slot);
}