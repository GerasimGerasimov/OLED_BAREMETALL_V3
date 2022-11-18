#ifndef INI_SLOTS_PROPS_H
#define INI_SLOTS_PROPS_H

#include "resources.h"
#include <vector>
#include <map>
#include "inistring.h"
#include "signal.h"

typedef struct {
	u16 Interval;
	u16 StartAddr;
	u16 LastAddr;
} TDeviceSlotProps;

typedef struct {
	std::string Source;
	std::string COM;
	u16 NetworkAddr;
	std::map<std::string, TDeviceSlotProps*> SlotsMap; //Key is RAM, FLASH, CD
} TDeviceNetworkProps;

class IniSlotsProps{
public:
	static void init(void);
	static std::string getSourceOfDev(std::string position);
	static u16 getDevNetWorkAddr(std::string position);
	static std::map<std::string, TDeviceNetworkProps> Devices;
private:
	static bool readDevices(void);
	static TDeviceNetworkProps getDeviceNetworkProps(char* src, int size);
};

#endif