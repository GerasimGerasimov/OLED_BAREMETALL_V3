#ifndef T_PAGE_HELP_H
#define T_PAGE_HELP_H

#include "Pages.h"
#include "TComponentListVertical.h"
#include "HeaderLabel.h"
#include "WrappedText.h"

class TPageHelp : public TPage
{
public:
    virtual void view();
    virtual const u16 getHeight(void) { return 0; };
    void onOpen();
    void startToClose();
    bool ProcessMessage(TMessage* m);
    TPageHelp(std::string Name);
    ~TPageHelp();
private:
    TComponentListVertical* MainMenu;
    THeaderLabel* pHeader;
    TWrappedText* pText;
};

#endif