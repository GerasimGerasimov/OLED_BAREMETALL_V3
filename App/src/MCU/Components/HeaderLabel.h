#ifndef HEADER_LABEL_H
#define HEADER_LABEL_H

#include <string>
#include "Label.h"
#include "graphics.h"

//строка текста
class THeaderLabel : public TLabel {
public:
    virtual void view(void);//вывести строку на экране
    THeaderLabel(TLabelInitStructure init);//конструктор
    ~THeaderLabel();//деструктор
};

#endif