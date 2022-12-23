#ifndef T_PAGE_MAIN_MENU_H
#define T_PAGE_MAIN_MENU_H

#include "Pages.h"
#include "TVerticalContainer.h"
#include "LinkLabel.h"
#include "Label.h"

class TPageMainMenu : public TPage
{
public:
    virtual void view();
    virtual const u16 getHeight(void) { return 0; };
    bool ProcessMessage(TMessage* m);
    TPageMainMenu(std::string Name);
    ~TPageMainMenu();
private:
    TVerticalContainer* Container;
};

#endif