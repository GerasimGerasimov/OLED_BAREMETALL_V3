#ifndef T_PAGE_OPERATE_STATUS_H
#define T_PAGE_OPERATE_STATUS_H

#include "Pages.h"
#include "TVerticalContainer.h"
#include "HandleSubscribers.h"

class TPageOperateStatus : public TPage
{
public:
    virtual void view();
    virtual const u16 getHeight(void) { return 0; };
    void onOpen();
    bool ProcessMessage(TMessage* m);
    void startToClose();
    TPageOperateStatus(std::string Name);
    ~TPageOperateStatus();
private:
    TVerticalContainer* Container;
    void SlotU1RAMUpdate(TSlotHandlerArsg args);
    TVisualObject* getSignalOfFocusedChild();
    int SubscriberID;
};

#endif