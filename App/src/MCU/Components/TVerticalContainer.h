#ifndef VERTICAL_CONTAINER_H
#define VERTICAL_CONTAINER_H

#include "TComponentsContainer.h"
#include "stm32f4xx.h"
#include "msg.h"
#include "graphics.h"

typedef struct TVerticalContainerProps {
    bool EnableScrollBar;
};

class TVerticalContainer : public TComponentsContainer
{
  public:
    bool isOpen;
    bool EnableScrollBar;
    //с какой по какую строку меню выводить, если кол-во строк не помещаетс€ на дисплее
    u16 FocusedLine;//строка наход€ща€с€ в фокусе
    u16 FirstPosition;//перва€ отображаема€ строка начинаю выводить с неЄ
    u16 LastPosition; //последн€€ отображаема€ строка
    virtual void view(void);//вывести объект на экране
    virtual const u16 getHeight(void);
    virtual void  Clear(void);//очистит список
    bool ProcessMessage(TMessage* m);//обработчик сообщений
    virtual TVisualObject* getFocusedElement();//дл€ контейнеров
    TVerticalContainer(TVerticalContainerProps& props, std::vector <TVisualObject*> Source = {});//конструктор
    ~TVerticalContainer();//деструктор
    virtual bool onEnter(void);
  private:
    u16 GetViewObjectsCount();//кол-во объектов умещающихс€ в высоту меню от FirstPosition до нижнего кра€
    u16 getScrollBarWidth();
    void goUp(void);
    void goDown(void);
    void drawScrollBar(void);
};

#endif