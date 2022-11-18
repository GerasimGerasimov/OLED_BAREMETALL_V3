#ifndef WRAPPED_TEXT_H
#define WRAPPED_TEXT_H

#include <string>
#include "common.h"
#include "graphics.h"
#include "Label.h"

//строка текста
class TWrappedText : public TVisualObject {
public:
    virtual bool ProcessMessage(TMessage* m);
    virtual void view(void);//вывести text на экране
    virtual const u16 getHeight(void);
    void setText(std::string text);//изменить текст в строке передав еЄ адрес
    TTextSizes getSize(void);
    TWrappedText(TLabelInitStructure init);//конструктор
    ~TWrappedText();//деструктор
private:
    std::string Font;
    std::vector<std::string> List;
    void outText();
    u16 GetViewObjectsCount();//кол-во объектов умещающихс€ в высоту меню от FirstPosition до нижнего кра€
    //u16 FocusLine;//строка наход€ща€с€ в фокусе
    u16 FirstPosition;//перва€ отображаема€ строка начинаю выводить с неЄ
    u16 LastPosition; //последн€€ отображаема€ строка
    void goUp(void);
    void goDown(void);
};

#endif