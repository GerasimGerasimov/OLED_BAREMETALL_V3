#ifndef T_PAGE_WARNINGS_H
#define T_PAGE_WARNINGS_H

#include "Pages.h"
#include "TVerticalContainer.h"

class TPageWarnings : public TPage
{
public:
    virtual void view();
    bool ProcessMessage(TMessage* m);
    TPageWarnings(std::string Name);
    ~TPageWarnings();
    void onOpen();
    void startToClose();
private:
    TVerticalContainer* Container;
    TVerticalContainer* AlarmsList;
    void fillPageContainer(void);
};

#endif