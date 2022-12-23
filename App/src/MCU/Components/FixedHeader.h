#ifndef FIXED_HEADER_H
#define FIXED_HEADER_H

#include <string>
#include "Label.h"
#include "graphics.h"

//строка текста
class TFixedHeader : public TLabel {
public:
    virtual void view(void);//вывести строку на экране
    TFixedHeader(TLabelInitStructure init);//конструктор
    ~TFixedHeader();//деструктор
    TVisualObject* Owner;
};

#endif