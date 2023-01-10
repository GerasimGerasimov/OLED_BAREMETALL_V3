#ifndef INI_RESOURCES_H
#define INI_RESOURCES_H

#include "resources.h"
#include <vector>
#include <map>
#include "inistring.h"
#include "signal.h"

typedef struct {
	std::string device;
	std::string section;
	std::string name;
} TValueSearchStruct;

class IniResources {
public:
	static void init(void);
	static ISignal* getSignalByTag(const std::string& tag);
	static std::string getDevicePositionByTag(const std::string& tag);
	static u16 getDevNetWorkAddrByTag(const std::string& tag);
	static u16 getDevNetWorkAddrByDevPos(const std::string& DevPosition);
	static std::string getScaleValueByKey(const std::string& key, const std::string& dev);
	static std::string getComment(const TValueSearchStruct& srch);
	static TValueSearchStruct spliceTagInfo(const std::string& tag);
private:
	static std::map<std::string, std::map<std::string, std::map<std::string, ISignal*>>> Sources;
	static bool readSources(void);
	static TValueSearchStruct TagToValueSearchStruct(const std::string& tag);
};

#endif