#ifndef TAG_LINE_H
#define TAG_LINE_H

#include "common.h"
#include "Label.h"
#include "Events.hpp"
#include "parameters.h"

//строка текста
class TTagLine : public TVisualObject {
public:
    virtual bool ProcessMessage(TMessage* m);
    virtual const u16 getHeight(void);
    TTagLine(std::string caption, std::string tag, TLabelInitStructure init);//конструктор
    ~TTagLine();//деструктор
    void view(void);//вывести строку на экране
    TLabel* Value;
    const std::string Tag;
    const std::string ComponentName();
private:
    TLabel* Caption;
    TLabel* msu; 
};

#endif