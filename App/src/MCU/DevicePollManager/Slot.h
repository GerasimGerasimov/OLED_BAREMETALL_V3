#ifndef SLOT_H
#define SLOT_H

#include <string>
#include <vector>
#include "stm32f4xx.h"
#include "icommastert.h"

#define MB_MASTER_BUF_SIZE  256
#define MB_MASTER_SLOTS_MAX 24

enum class RespondErrorCodes : s16 {
	LNK = -1,
	CRCE = -2,
	CMD = -3
};

enum class SlotStateFlags {
	REPEATING_TRANSMIT = 1,
	SKIP_SLOT = 2,
	TIMEOUT_ERR = 4,
	CRC_ERR = 8,
	DATA_VALID = 16
};

class Slot {
	using TSlotDataHandler = std::function<void(Slot& slot, u8* reply)>;
public:
	u16 Flags;
	u16 RespondLenghtOrErrorCode;
	virtual void init(void);
	void addcmd(u8 cmd[], u8 size);
	void addcmd(const std::vector<u8> &v);
	void validation(s16 result, u8* reply);
	Slot();
	Slot(std::string device, std::string section, u16 StartAddr, u16 LastAddr);
	~Slot();
	u8 cmdLen;
	u8 OutBuf[MB_MASTER_BUF_SIZE];//буфер сообщения для отправки
	u8 InputBuf[MB_MASTER_BUF_SIZE];//буфер сообщения для приёма сообщений
	u8 InputBufValidBytes;
	TSlotDataHandler onData;//процедура которая вызывается после получения ответа от устройства, если контрольная сумма сошлась
	u16 StartAddrOffset;
	u16 LastAddrOffset;
	u16 Interval;
	u16 DelayAfterWrite;
	std::string Device;
	std::string Section;
	bool isIntervalDone();
private:
	bool isReplyCRCValid(s16 result, u8* reply);
	u16 TmpInterval;
};

#endif