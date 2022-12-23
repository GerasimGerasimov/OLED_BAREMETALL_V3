#ifndef FIXED_FOOTER_H
#define FIXED_FOOTER_H

#include <string>
#include "Label.h"
#include "graphics.h"

//строка текста
class TFixedFooter : public TLabel {
public:
    virtual void view(void);//вывести строку на экране
    TFixedFooter(TLabelInitStructure init);//конструктор
    ~TFixedFooter();//деструктор
    TVisualObject* Owner;
};

#endif