#ifndef VARS_H
#define VARS_H

#include "stm32f4xx.h"
#include <vector>
#include <string>
#include "signal.h"

typedef struct {
	char* pKey;
	char* pValue;
	int	ValueSize;
} TScaleProps;

class TScale : public ISignal {
public:
	TScale(char* source, int scrLen);
	TScale(TScaleProps props);
	~TScale();
	std::string getName();
	std::string getValue();
protected:
	int	ValueSize;
	std::string getValueFromList(std::vector<std::string> values);
};

#endif