#ifndef LINK_LABEL_H
#define LINK_LABEL_H

#include "Label.h"
#include "Events.hpp"

//строка текста с горизонтальной автоматической прокруткой
class TLinkLabel : public TLabel {
public:
    virtual bool ProcessMessage(TMessage* m);
    TLinkLabel(std::string caption, std::string url, TLabelInitStructure init);//конструктор
    ~TLinkLabel();//деструктор
private:
    std::string URL;
    std::string SrcCaption;
    int Shift;
    void doShift(void);
};

#endif