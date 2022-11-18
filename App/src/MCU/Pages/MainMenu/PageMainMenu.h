#ifndef T_PAGE_MAIN_MENU_H
#define T_PAGE_MAIN_MENU_H

#include "Pages.h"
#include "TComponentListVertical.h"
#include "LinkLabel.h"
#include "Label.h"

class TPageMainMenu : public TPage
{
public:
    virtual void view();//вывести объект на экране
    virtual const u16 getHeight(void) { return 0; };
    void clear();//очистит список
    bool ProcessMessage(TMessage* m);//обработчик сообщений
    TPageMainMenu(std::string Name);//конструктор
    ~TPageMainMenu();//деструктор
private:
    TComponentListVertical* MainMenu;
};

#endif