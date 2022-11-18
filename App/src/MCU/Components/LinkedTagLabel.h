#ifndef LINKED_TAG_LABEL_H
#define LINKED_TAG_LABEL_H

#include "LinkLabel.h"
#include "Events.hpp"
#include "signal.h"

//строка текста
class TLinkedTagLabel : public TLinkLabel {
public:
    virtual bool ProcessMessage(TMessage* m);
    TLinkedTagLabel(std::string tag, TLabelInitStructure init);//конструктор
    ~TLinkedTagLabel();//деструктор
    void view(void);//вывести строку на экране
private:
    ISignal* DataSrc;
};

#endif