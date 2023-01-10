#include "CmdSender.h"
#include <AppModbusSlave.h>
#include "Alarms.h"

u16 CmdSender::keyStopReset = 0;
u16 CmdSender::keyRun = 0;
u16 CmdSender::keyClean = 0;
u16 CmdSender::keyVAC = 0;
u16 CmdSender::keyPrevStopReset = 0;
u16 CmdSender::keyPrevRun = 0;
u16 CmdSender::keyMode = 0;
u16 CmdSender::keyPrevMode = 0;
bool CmdSender::cmdSendInProcess = false;
u16 CmdSender::TryCount = 3;

static const std::string CMD_RUN = "1111";
static const std::string CMD_STOP = "1000";
static const std::string CMD_RESET = "2222";

static const std::string CMD_NORMAL = "5310";
static const std::string CMD_CLEAN = "5311";
static const std::string CMD_VAC = "5312";
static const std::string CMD_TEST = "5313";

static const u16 SELECT_CMD_NORMAL = 0;
static const u16 SELECT_CMD_CLEAN = 1;
static const u16 SELECT_CMD_VAC = 2;
static const u16 SELECT_CMD_MANU = 3;

void CmdSender::init() {

}

void CmdSender::update(const u16 din) {
	updateKeyRun(din);
	updateKeyStop(din);
	updateKeyMode(din);
}

void CmdSender::updateKeyMode(const u16 din) {
	keyClean = (din & (1 << 2));
	keyVAC   = (din & (1 << 3));
	keyMode = SELECT_CMD_NORMAL;
	if (keyClean) keyMode = SELECT_CMD_CLEAN;
	if (keyVAC)   keyMode = SELECT_CMD_VAC;
	if ((keyClean !=0) && (keyVAC !=0)) keyMode = SELECT_CMD_MANU;
	if (keyPrevMode != keyMode) {
		/*TODO тут бы использовать таймер, чтобы чтобы при переключении,
		  использовать фиксированное (за время) значение*/
		if (cmdSendInProcess != true) {
			switch (keyMode) {
			case SELECT_CMD_NORMAL:
				sendCmd((std::string&)CMD_NORMAL);
				break;
			case SELECT_CMD_CLEAN:
				sendCmd((std::string&)CMD_CLEAN);
				break;
			case SELECT_CMD_VAC:
				sendCmd((std::string&)CMD_VAC);
				break;
			case SELECT_CMD_MANU:
				sendCmd((std::string&)CMD_TEST);
				break;
			}
			keyPrevMode = keyMode;
		}
	}
}

void CmdSender::updateKeyStop(const u16 din) {
	keyStopReset = (din & (1 << 0));
	if (keyPrevStopReset != keyStopReset) {
		if (keyStopReset) {
			if (cmdSendInProcess != true) {
				if (Alarms::isAlarm()) {
					sendCmd((std::string&)CMD_RESET);
				}
				else {
					sendCmd((std::string&)CMD_STOP);
				}
			}
		}
		keyPrevStopReset = keyStopReset;
	}
}

void CmdSender::updateKeyRun(const u16 din) {
	keyRun = (din & (1 << 1));
	if (keyPrevRun != keyRun) {
		if (keyRun) {
			if (cmdSendInProcess != true) {
				sendCmd((std::string&) CMD_RUN);
			}
		}
		keyPrevRun = keyRun;
	}
}

void CmdSender::sendCmd(std::string& code) {
	std::string cmd = "U1/RAM/CMD/";
	TryCount = 3;
	cmdSendInProcess = true;
	ModbusSlave::setValue(cmd, code, SlotUpdate);
}

void CmdSender::SlotUpdate(Slot& slot, u8* reply) {
	if (slot.RespondLenghtOrErrorCode) {
		slot.Flags |= (u16)SlotStateFlags::SKIP_SLOT;
		cmdSendInProcess = false;
	}
	else {
		if (TryCount)
			TryCount--;
		else {
			slot.Flags |= (u16)SlotStateFlags::SKIP_SLOT;
			cmdSendInProcess = false;
		}
	}
	/*TODO разделить использование слота с PageEditValue*/
}