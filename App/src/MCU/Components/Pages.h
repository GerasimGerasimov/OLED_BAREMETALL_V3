#ifndef T_PAGE_H
#define T_PAGE_H

#include "TComponentsContainer.h"
#include "stm32f4xx.h"
#include "msg.h"

class TPage : public TComponentsContainer
{
public:
    bool isOpen;
    //с какой по какую строку меню выводить, если кол-во строк не помещается на дисплее
    TVisualObject* pFocusedElement;//элемент находящийся в фокусе
    virtual void view();//вывести объект на экране
    virtual const u16 getHeight(void) { return 0; };
    virtual void onOpen();
    void clear();//очистит список
    bool ProcessMessage(TMessage* m);//обработчик сообщений
    TPage(std::string name);//конструктор
    TPage(std::string name, bool isOpenState, std::vector <TVisualObject*> Source = {});//конструктор
    ~TPage();//деструктор
    void setProps(void* _props);
    virtual void startToClose();
    virtual bool isClosed();
protected:
    void* props;
    std::string Name;
    std::string BackPage;
};

#endif