#include "parameters.h"
#include <parser.h>

TParameter::TParameter(char* source, int scrLen) 
	: ISignal(source, scrLen)
	, Comment(NULL)
	, MSU(NULL) {
}

TParameter::TParameter(TSignalPropsPointers props)
	: ISignal(props)
	, Comment(props.pComment)
	, MSU(NULL) {
}

TParameter::~TParameter(){
}

std::string TParameter::getComment() {
	return (Comment)
		? IniParser::getElement('/', Comment)
		: "";
}

/*
Где находится MSU

//Special case One
p19802=P/C/TPrmList/xF0C9/r2064.H/msu//x03#9600/x04#19200/x05#57600/x06#115200/const/

//Special case Two
p13321=P/C/TBit    /xF042/mask/r2021.1/1/1/const/ вообще нет MSU

//General case
//общий случай для MSU.. но TODO сначала надо распарсить option там содержится msu 
p65301=P/C/TByte   /xE06A/rC035.L/msu/scale/bytes//const/
p17100=P/C/TInteger/xF090/r2048  /msu/scale/bytes//const/
p60200=P/C/TWORD   /xE004/rC002  /msu/scale/bytes//const/
p60000=P/C/TDWORD  /xE000/rC000  /msu/scale/bytes//const/
p62800=P/C/TFloat  /xE038/rC01C  /msu/scale/bytes//const/
*/

std::string TParameter::getMSU()
{
	return (MSU)
		? IniParser::getElement('/', MSU)
		: "";
}