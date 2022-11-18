#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "stm32f4xx.h"
#include <vector>
#include <string>
#include "signal.h"

class TParameter : public ISignal{
public:
	TParameter(char* source, int scrLen);
	TParameter(TSignalPropsPointers props);
	~TParameter();
	std::string getComment();
	std::string getMSU();
protected:
	char* Comment;
	char* MSU;
};

#endif