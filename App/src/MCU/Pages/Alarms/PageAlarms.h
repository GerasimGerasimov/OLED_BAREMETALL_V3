#ifndef T_PAGE_ALARMS_H
#define T_PAGE_ALARMS_H

#include "Pages.h"
#include "TVerticalContainer.h"

class TPageAlarms : public TPage
{
public:
    virtual void view();
    bool ProcessMessage(TMessage* m);
    TPageAlarms(std::string Name);
    ~TPageAlarms();
    void onOpen();
    void startToClose();
private:
    TVerticalContainer* Container;
    TVerticalContainer* AlarmsList;
    void fillPageContainer(void);
    u16 EmptyViewDelay;
};

#endif