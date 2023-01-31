#ifndef PARAM_LIST_SELECTOR_H
#define PARAM_LIST_SELECTOR_H

#include <string>
#include "common.h"
#include "graphics.h"
#include "Label.h"

class TParamListSelector : public TVisualObject {
public:
    bool isSingle;
    virtual void view(void);//вывести строку на экране
    virtual const u16 getHeight(void);
    void setCaption(std::string caption);
    std::string getValue(void);
    bool ProcessMessage(TMessage* m);
    TTextSizes getSize(void);
    TParamListSelector(TLabelInitStructure init);//конструктор
    ~TParamListSelector();//деструктор
protected:
    std::string Font;
    std::string Caption;
    TTextSizes TextSize;
    TColorScheme PrimaryColor; //основная цветовая схема 
    TColorScheme SelectedColor;//цветовая схема при выделении
    int Style;
    void outCaption(TColorScheme& ColorScheme);
    s16 getCaptionLeftPosition(void);
    void valueUp(void);
    void valueDown(void);
    int Position;
    void fillIntFracArrays(std::string& cap);
};

#endif