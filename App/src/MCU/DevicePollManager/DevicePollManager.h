#ifndef DEVICE_POLL_MANAGER_H
#define DEVICE_POLL_MANAGER_H

#include "stm32f4xx.h"
#include "com_master_driver.h"
#include "Slot.h"
#include <vector>

#define MB_MASTER_BUF_SIZE  256
#define MB_MASTER_SLOTS_MAX 24

enum class DevicePollManagerStatus {
	WAIT_IDLE = 1,
	WAIT_RESPOND = 2,
	PARSE_RESPOND = 3,
	TOGGLE_SLOT = 4,
	SEND_REQUEST = 5
};

class DevicePollManager {
public:
	static void init(std::vector <Slot> slots);
	static void execute(void);
	static std::vector <Slot> Slots;
	static u16 Status;
	static Slot* getSlotByDevPosAndSection(const std::string& device, const std::string& section);
private:
	static void checkRespond(s16 result, u8* reply);
	static Slot* slot;
	static u16 idx;
	static Slot* getNextSlot(void);
	static DevicePollManagerStatus setActionBySlot(void);
	static u8 Reply[256];
	static s16 ReplyResult;

};

#endif